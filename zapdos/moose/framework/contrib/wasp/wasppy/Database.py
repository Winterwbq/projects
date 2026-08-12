from wasp import *
from io import StringIO

class DeserializedResult:
    '''Stores both input node (providence), action results (user data), and value default
    '''
    def __init__(self, node:WaspNode, interpreter:Interpreter, action=None):
        self.action = action
        self.node = node
        self.interpreter = interpreter
        self.userData:'dict{str:list(DeserializedResult)}' = {}

    def isTerminal(self):
        '''Determine if this deserialized result is terminal. I.e., it has no children data'''
        return len(self.node) == 0

    @staticmethod
    def fromDefault(node:WaspNode, interpreter:Interpreter, default, key="value"):
        '''
            Create a result from a default. This creates the equivalent
            {"value":{":=":default}}
            Later accessible as [key].value() or valuelist()
        '''
        if type(default) is not list:
            # scalar default
            dr = DeserializedResult(node, interpreter)
            cdr = DeserializedResult(node, interpreter)
            dr.userData[key] = cdr
            cdr.store(default)
        elif type(default) is list:
            # list default
            dr = DeserializedResult(node, interpreter)
            result = []
            for d in default:
                cdr = DeserializedResult(node, interpreter)
                cdr.store(d)
                result.append(cdr)
            dr.userData[key] = result
        else:
            # unknown, not implemented
            raise NotImplementedError("Type of default!")
        return dr

    def addResult(self, result:'DeserializedResult', inputObject):
        '''Append the deserialize result to existing userData dictionary
           mapping node name to user data

           If the result is expected to have a MaxOccurs of more than 1
           a list is created, if needed, and the result appended
           '''
        name = result.node.name()
        maxOccurs = inputObject.maxOccurs(name)
        # MaxOccurs of None is NoLimit
        aScalar = maxOccurs is not None and maxOccurs == 1
        if name not in self.userData and not aScalar:
            self.userData[name] = []

        if aScalar:
            if name in self.userData:
                result.interpreter.createErrorDiagnostic(result.node, "occurrence exceeds maximum allowed occurrence of "+str(maxOccurs)+"!")
            else:
                self.userData[name] = result
        else:
            self.userData[name].append(result)
            occurrence = len(self.userData[name])
            if  maxOccurs is not None and occurrence > maxOccurs:
                result.interpreter.createErrorDiagnostic(result.node, "has " + str(occurrence) + " occurrences which exceeds max occurs of " + str(maxOccurs) + "!")

        return result

    def select(self, context:str):
        '''Obtain the deserialized result associated with the given context lookup
           context:str - the path to a child context. E.g., 'x/y/z' where z must be a terminal result
           return:list|None - the list of deserialized selection
        '''
        lineage = context.split("/")
        current = [self]
        next = []

        for name in lineage:
            while len(current) > 0:
                # wild card ignores
                if name == "*":
                    # Append all
                    for result in current[-1].userData.values():
                        if type(result) is list: next.extend(reversed(result))
                        else: next.append(result)
                elif name in current[-1].userData:
                    # Append only those that name match (non-null result)
                    result = current[-1].userData[name]
                    if result and type(result) is list:
                        next.extend(reversed(result))
                    elif result: next.append(result)
                current.pop()
            # Reverse order to preserve user-input order
            next.reverse()
            # Update current result being searched to be those identified
            current = next
            next = []

        return current if len(current) > 0 else None

    def store(self, value, key=None):
        if key is None: # default to ':=' scalar store
            key = ":="
        if key in self.userData and type(self.userData) is not list:
            self.userData[key] = [self.userData[key], value]
        else:
            self.userData[key] = value
        return value

    def storedResult(self, key=None):
        if key is None: key = ":="
        return self.userData[key] if key in self.userData else None

    def todict(self):
        '''Produce a dictionary representation of this deserialized result
           Stored results will have a key of ':='
        '''
        d = {}
        for key, value in self.userData.items():
            if type(value) is list:
                d[key] = []
                for v in value:
                    d[key].append(v.todict())
            elif type(value) is DeserializedResult:
                d[key] = value.todict()
            else:
                d[key] = value
        return d

    def hasValue(self, vkey="value"):
        dr = self.userData[vkey]
        return dr is not None and type(dr) is DeserializedResult and ":=" in dr.userData

    def value(self, vkey="value"):
        '''Obtain the associated 'value' nodes stored result '''
        dr = self.userData[vkey]
        assert dr is not None and type(dr) is DeserializedResult and ":=" in dr.userData, "value request requirements not met!"
        return dr.storedResult()

    def valuelist(self, vkey="value"):
        '''Convert the value of this DeserializedResult to a list.
        I.e., each deserialized stored result is converted to a list
        This is called for array user data. Because each 'value' is
        an input node with provedence, the ability to obtain a convenient
        list of the data is preferred.
        '''
        l = []
        assert vkey in self.userData, "valuelist request requirements not met!"
        for dr in self.userData[vkey]:
            l.append(dr.storedResult())
        return l


    def __bool__(self):
        '''Is this result valid based on if it has a node and an interpreter'''
        return self.node is not None and self.interpreter is not None

    def __len__(self):
        '''Obtain the length of this deserialized result.
        This will be the length of the stored result
        or if not a stored result (i.e., deserialized parent node) the number of
        child nodes '''
        if sr := self.storedResult():
            if type(sr) is list:
                return len(sr)
            else:
                return 1
        return len(self.userData)

    def __getitem__(self, key):
        '''Obtain the named deserialized result or if
        this is only a stored result, the result'''
        if key in self.userData:
            d = self.userData[key]
            # check if this items is only a storedResult (":=") and return it
            if type(d) is DeserializedResult and d.storedResult() is not None:
                return d.storedResult()
            return d
        # return null/invalid result
        # allows arbitrarily nested requests
        # without failing
        return DeserializedResult(None, None)


class InputObject:
    '''Generic Input object type used to track object requirements'''

    def __init__(self, **kwargs):
        '''
            Action - function pointer to be executed
            Desc: str - the description of this input object
            Enums:list(str) - enumerated values allowed
            MaxValExc:float - the maximum exclusive value for this input object
            MaxValInc:float - the maximum inclusive value for this input object
            MinValExc:float - the minimum exclusive value for this input object
            MinValInc:float - the minimum inclusive value for this input object
        '''

        self._action = kwargs.pop("Action", None)
        self._children = None  # dict(childKey:childObject)
        self._defaults = None  # dict(childKey, default)
        self._description  = kwargs.pop("Desc", None)
        self._enums = kwargs.pop("Enums", None)
        self._maxOccurs = None # dict(childKey:maxOccurs)
        self._maxValExc = kwargs.pop("MaxValExc", None)
        self._maxValInc = kwargs.pop("MaxValInc", None)
        self._minOccurs = None # dict(childKey:minOccurs)
        self._minValExc = kwargs.pop("MinValExc", None)
        self._minValInc = kwargs.pop("MinValInc", None)
        self._atmost    = None
        self._atleast   = None
        self._exactly   = None
        self._unique    = None # list(list(str)): list of path context to data which must be unique
        self._exists    = None

        assert len(kwargs) == 0, "Unexpected additional parameters to InputObject: " + str(kwargs)

    def isTerminal(self):
        '''Determine if this InputObject does not have children
           Return true, iff there are no children data
        '''
        # This is terminal if children is None
        return self._children is None

    def action(self):
        '''Obtain the action associated with the given child input key'''
        return self._action

    def _pre_add(self, inputKey, **kwargs):
        if self._children is None:
            self._children = {}
            self._defaults = {}
            self._maxOccurs = {}
            self._minOccurs = {}

        assert inputKey not in self._children

    def add(self, inputKey, inputObject, **kwargs):
        '''Add an exissting object as a child

        '''
        self._pre_add(inputKey, **kwargs)
        if "MaxOccurs" in kwargs: self._maxOccurs[inputKey] = kwargs.pop("MaxOccurs")
        if "MinOccurs" in kwargs: self._minOccurs[inputKey] = kwargs.pop("MinOccurs")
        if "Default" in kwargs: self._defaults[inputKey] = kwargs.pop("Default")
        self._children[inputKey] = inputObject
        return self._children[inputKey]

    def addSingle(self, inputKey, inputObject, **kwargs):
        kwargs["MaxOccurs"] = 1
        return self.add(inputKey, inputObject, **kwargs)

    def addRequired(self, inputKey, inputObject, **kwargs):
        kwargs["MinOccurs"] = 1
        return self.add(inputKey, inputObject, **kwargs)

    def addRequiredSingle(self, inputKey, inputObject, **kwargs):
        kwargs["MaxOccurs"] = 1
        kwargs["MinOccurs"] = 1
        return self.add(inputKey, inputObject, **kwargs)

    def select(self, context:str):
        '''Obtain the input objects associated with the given context lookup
           context:str - the path to a child context. E.g., 'x/y/z' where z must be a terminal object
           return:list|None - the list of input object selection
        '''
        lineage = context.split("/")
        current = [self]
        next = []
        for name in lineage:
            while len(current) > 0:
                # wild card ignores
                if current[-1]._children is not None:
                    if name == "*":
                        # Append all
                        for inputobject in current[-1]._children.values():
                            next.append(inputobject)
                    else:
                        # Append only those that name match
                        child = current[-1][name]
                        if child:
                            next.append(child)
                current.pop()
            # Reverse order to preserve user-input order
            next.reverse()
            # Update current objects being searched to be those identified
            current = next
            next = []

        return current if len(current) > 0 else None

    def addAtMostConstraint(self, am:'SourcePredicatedTarget'):
        '''Add an AtMostConstraint to this object

            Raises: exception if referenced targets do not exist or are not terminal
        '''
        assert am._source is None or self.select(am._source), "Unable to verify at most constraint for source! Constraint must reference an existing component!"
        # Check that target exist and are terminal
        for t, v in am._target.items():
            selected = self.select(t)
            assert selected is not None, "Unable to verify at most constraint for "+t+"! Constraints must refer to existing component!"
            # If no value is associated with the target (i.e., targets[t] = [None])
            # skip checking that the component is terminal (i.e., will have a value)
            if v == [None] or v == None: continue
            # Ensure the component is terminal
            for component in selected:
                # Unique constraints must reference terminal objects for comparison of data
                assert component.isTerminal(), \
                "attempting to add AtMost terminal constraints to "+t+" which is not a terminal component!"

        if self._atmost == None:
            self._atmost = []
        self._atmost.append(am)

    def addAtLeastConstraint(self, al:'SourcePredicatedTarget'):
        '''Add an AtLeastConstraint to this object

            Raises: exception if referenced targets do not exist or are not terminal
        '''
        assert al._source is None or self.select(al._source), "Unable to verify at least constraint for source! Constraint must reference an existing component!"
        # Check that target exist and are terminal
        for t, v in al._target.items():
            selected = self.select(t)
            assert selected is not None, "Unable to verify at least constraint for "+t+"! Constraints must refer to existing component!"
            # If no value is associated with the target (i.e., targets[t] = [None])
            # skip checking that the component is terminal (i.e., will have a value)
            if v == [None] or v == None: continue
            # Ensure the component is terminal
            for component in selected:
                # Unique constraints must reference terminal objects for comparison of data
                assert component.isTerminal(), \
                "attempting to add AtLeast terminal constraints to "+t+" which is not a terminal component!"

        if self._atleast == None:
            self._atleast = []
        self._atleast.append(al)

    def addExactlyConstraint(self, exactly:'SourcePredicatedTarget'):
        '''Add an Eactly constraint to this object

            Raises: exception if referenced targets do not exist or are not terminal
        '''
        assert exactly._source is None or self.select(exactly._source), "Unable to verify exactly constraint for source! Constraint must reference an existing component!"
        # Check that target exist and are terminal
        for t, v in exactly._target.items():
            selected = self.select(t)
            assert selected is not None, "Unable to verify exactly constraint for "+t+"! Constraints must refer to existing component!"
            # If no value is associated with the target (i.e., targets[t] = [None])
            # skip checking that the component is terminal (i.e., will have a value)
            if v == [None] or v == None: continue
            # Ensure the component is terminal
            for component in selected:
                # Unique constraints must reference terminal objects for comparison of data
                assert component.isTerminal(), \
                "attempting to add Exactly terminal constraints to "+t+" which is not a terminal component!"

        if self._exactly == None:
            self._exactly = []
        self._exactly.append(exactly)

    def getExistsConstraintTargetGivenSource(self, source_path:'str'):
        '''Given the candidate source path (e.g., where an id is used), obtain the target paths (e.g., where the id is defined)
           Returns list(str): list of ExistsConstraint target paths relative to this InputObject
           Returns None: if no ExistsConstraint with given source is found

        '''
        # Get all node path parts, minus empty strings (can occur if '/' is prefix or suffix)
        source_parts = list(filter(None, source_path.split("/")))
        # capture current object's ExistsConstraint, if present
        current_target = []
        if self._exists: [current_target.extend(ec._target) for ec in self._exists if source_path in ec._source]
        # For every part (child InputObject)
        # if child with given name doesn't exist, return current list or None
        # if child exists, search if the child contains ExistsContaint with given target
        # - if constraint exists, capture absolute path to constaint target (path-to-current-object + target_path)
        current_path = ""
        current_object = self 
        targets = current_target if len(current_target) > 0 else None
        for child_name in source_parts:
            # child isn't named in current object
            if child_name not in current_object:
                return targets # either None or what we found in parent objects
            current_object = current_object[child_name]
            current_target = []
            current_path += child_name+"/"
            relative_source_path = source_path.removeprefix(current_path)
            if current_object._exists: [current_target.extend([relative_source_path+"/"+t  for t in ec._target]) for ec in current_object._exists if relative_source_path in ec._source]
            if len(current_target) > 0: targets.extend(current_target)
        return targets
    
    def getExistsConstraintSourceGivenTarget(self, target_path:'str'):
        '''Given the candidate target path (e.g., where an id is defined), obtain the source paths (e.g., where the id is used)
           Returns list(str): list of ExistsConstraint source paths relative to this InputObject
           Returns None: if no ExistsConstraint with given target is found

        '''
        # Get all node path parts, minus empty strings (can occur if '/' is prefix or suffix)
        target_parts = list(filter(None, target_path.split("/")))
        # capture current object's ExistsConstraint, if present
        current_source = []
        if self._exists: [current_source.extend(ec._source) for ec in self._exists if target_path in ec._target]
        # For every part (child InputObject)
        # if child with given name doesn't exist, return current list or None
        # if child exists, search if the child contains ExistsContaint with given source
        # - if constraint exists, capture absolute path to constaint source (path-to-current-object + source_path)
        current_path = ""
        current_object = self 
        sources = current_source if len(current_source) > 0 else None
        for child_name in target_parts:
            # child isn't named in current object
            if child_name not in current_object:
                return sources # either None or what we found in parent objects
            current_object = current_object[child_name]
            current_source = []
            current_path += child_name+"/"
            relative_target_path = target_path.removeprefix(current_path)
            if current_object._exists: [current_source.extend([relative_target_path+"/"+s  for s in ec._source]) for ec in current_object._exists if relative_target_path in ec._target]
            if len(current_source) > 0: sources.extend(current_source)
        return sources

    def addExistsConstraint(self, ec:'ExistsConstraint'):
        '''Add an ExistsConstraint to this object

            Raises: exception if referenced source or target do not exist or are not terminal
        '''
        # Check that source and target exist and are terminal
        for s in ec._source:
            selected = self.select(s)
            assert selected is not None, "Unable to verify exist constraint for "+s+"! Constraints must refer to existing component!"
            for component in selected:
                # Unique constraints must reference terminal objects for comparison of data
                assert component.isTerminal(), "Exists constraints can only be applied to terminal data components!"

        for t in ec._target:
            selected = self.select(t)
            assert selected is not None, "Unable to verify exists constraint for "+t+"! Constraints must refer to existing component!"
            for component in selected:
                # Unique constraints must reference terminal objects for comparison of data
                assert component.isTerminal(), "Exists constraints can only be applied to terminal data components!"

        if self._exists == None:
            self._exists = []
        self._exists.append(ec)

    def addUniqueConstraint(self, context:'list(str)'):
        '''Add a uniqueness constraint to this input object that requires all referenced context to
        have different data.

        context:list(str) - the paths associated with the data which must be unique
                            E.g., 'x/y/z' means the x child, y granchild, and z greatgrandchild definitions must exist

        Note: This must be called after all context have been created

        Raises: exception if referenced context does not exist
        '''
        # Check each context selects input definition
        for c in context:
            selected = self.select(c)
            assert selected is not None, "Unable to verify uniqueness constraint for "+c+"! Constraints must refer to existing InputObject definitions"
            for inputObject in selected:
                # Unique constraints must reference terminal objects for comparison of data
                assert inputObject.isTerminal(), "Uniqueness constraints for "+c+" can only be applied to terminal data nodes!"

        if self._unique is None:
            self._unique = []
        self._unique.append(context)

    def create(self, inputKey, **kwargs):
        '''Create an object as a child

            Action - function pointer to be executed
            Enums - enumerated values allowed for the child
            Default - the default value if the child is not specified
            Desc - description of the child
            MaxOccurs - maximum occurrence of the child
            MinOccurs - minimum occurrence of the child
        '''
        self._pre_add(inputKey, **kwargs)
        if "MaxOccurs" in kwargs: self._maxOccurs[inputKey] = kwargs.pop("MaxOccurs")
        if "MinOccurs" in kwargs: self._minOccurs[inputKey] = kwargs.pop("MinOccurs")
        if "Default" in kwargs: self._defaults[inputKey] = kwargs.pop("Default")
        inputObject = InputObject(**kwargs)
        return self.add(inputKey, inputObject)

    def createSingle(self, inputKey, **kwargs):
        kwargs["MaxOccurs"] = 1
        return self.create(inputKey, **kwargs)

    def createRequired(self, inputKey, **kwargs):
        kwargs["MinOccurs"] = 1
        return self.create(inputKey, **kwargs)

    def createRequiredSingle(self, inputKey, **kwargs):
        kwargs["MaxOccurs"] = 1
        kwargs["MinOccurs"] = 1
        return self.create(inputKey, **kwargs)

    def _conductAvailableChildChecks(self, childKey, dr:DeserializedResult):
        occurrences = 0
        data = dr[childKey]
        data_type = type(data)
        if data_type is list or data_type is DeserializedResult:
            occurrences = len(data)
        else: # the data
            occurrences = 1
        # min occurs
        mio = self.minOccurs(childKey)
        if  mio and occurrences < mio:
            dr.interpreter.createErrorDiagnostic(dr.node,
                "has "+str(occurrences)+" occurrences of "+childKey+" when "+str(mio)+" are required!")

        # max occurs check was completed in DeserializedResult.addResult

    def _conductChecks(self, dr:DeserializedResult):
        storedResult = dr.storedResult()

        if storedResult is None:
            return

        # Conduct check of enumerated values
        if enums := self._enums:
            if type(storedResult) is list:
                for result in storedResult:
                    if str(result).lower() not in (enum.lower() for enum in enums):
                        dr.interpreter.createErrorDiagnostic(dr.node,
                            "value of "+str(result)+" is not one of the allowed values "+str(enums)+"!")
            elif str(storedResult).lower() not in (enum.lower() for enum in enums):
                dr.interpreter.createErrorDiagnostic(dr.node,
                    str(storedResult)+" is not one of the allowed values "+str(enums)+"!")

        if self._minValInc is not None and float(storedResult) < self._minValInc:
            dr.interpreter.createErrorDiagnostic(dr.node,
                str(storedResult)+" is less than the allowed minimum inclusive value of "+str(self._minValInc)+"!")

        if self._maxValInc is not None and float(storedResult) > self._maxValInc:
            dr.interpreter.createErrorDiagnostic(dr.node,
                str(storedResult)+" is greater than the allowed maximum inclusive value of "+str(self._maxValInc)+"!")

        if self._minValExc is not None and float(storedResult) <= self._minValExc:
            dr.interpreter.createErrorDiagnostic(dr.node,
                str(storedResult)+" is less than or equal to the allowed minimum exclusive value of "+str(self._minValExc)+"!")

        if self._maxValExc is not None and float(storedResult) >= self._maxValExc:
            dr.interpreter.createErrorDiagnostic(dr.node,
                str(storedResult)+" is greater than or equal to the allowed maximum exclusive value of "+str(self._maxValExc)+"!")

    def _conductUniquenessChecks(self, dr: DeserializedResult):
        # Each entry in _unique is a list of context paths
        # for which all data must be unique
        if self._unique is None: return
        for context in self._unique:
            errors = set() # set for tracking error emissions
            # accumulate all context
            all_context = []
            for c in context:
                selection = dr.select(c)
                if selection: all_context.extend(selection)
            count = len(all_context)
            for i, dri in enumerate(all_context):
                for j in range(i+1, count):
                    drj = all_context[j]
                    driv = dri.storedResult()
                    drjv = drj.storedResult()
                    if str(driv) == str(drjv):
                        if dri not in errors:
                            dr.interpreter.createErrorDiagnostic(dri.node, str(driv)+" must be unique but is duplicate to "+drj.node.info())
                            errors.add(dri)
                        if drj not in errors:
                            dr.interpreter.createErrorDiagnostic(drj.node, str(drjv)+" must be unique but is duplicate to "+dri.node.info())
                            errors.add(drj)

    def _conductExistsChecks(self, dr:DeserializedResult):
        if self._exists is None: return
        for constraint in self._exists:
            source = constraint.sources(dr)
            target = constraint.targets(dr)
            for s in source:
                sv = str(s.storedResult())
                targets = []
                for t in target:
                    if type(t) is DeserializedResult:
                        tv = str(t.storedResult())
                    else:
                        tv = t
                    targets.append(tv)
                if sv.lower() not in (tval.lower() for tval in targets):
                    origin = None
                    if s.node.child_count() == 0:
                        origin = str(s.node)
                    else:
                        origin = str(sv)
                    dr.interpreter.createErrorDiagnostic(s.node, origin+" is not one of the required existing targets! Required existing targets are "+", ".join(targets)+"!")

    def _conductAtMostChecks(self, dr:DeserializedResult):
        if self._atmost is None: return

        for constraint in self._atmost:
            source = constraint.source(dr)
            # The targets are predicated on source existing
            # Skip target constaint logic if source is None
            if source is None:
                continue
            context = "" if type(source) is not list else " when " + source[0].node.info() + " is present"
            targets = constraint.targets(dr)
            if (len(targets) > constraint.count()):
                dr.interpreter.createErrorDiagnostic(dr.node,
                            "has more than "+str(constraint.count())+" of: ["
                            +", ".join(targets)+"] - at most "+str(constraint.count())+" must occur" + context + "!")

    def _conductAtLeastChecks(self, dr:DeserializedResult):
        if self._atleast is None: return

        for constraint in self._atleast:
            source = constraint.source(dr)
            # The targets are predicated on source existing
            # Skip target constaint logic if source is None
            if source is None:
                continue
            targets = constraint.targets(dr)
            context = "" if type(source) is not list else " when " + source[0].node.info() + " is present"
            if (len(targets) < constraint.count()):
                dr.interpreter.createErrorDiagnostic(dr.node,
                            "has fewer than "+str(constraint.count())+" of: ["
                            +", ".join(constraint.targets_predicated())+"] - at least " + str(constraint.count())+" must occur" + context + "!")

    def _conductExactlyChecks(self, dr:DeserializedResult):
        if self._exactly is None: return

        for constraint in self._exactly:
            source = constraint.source(dr)
            # The targets are predicated on source existing
            # Skip target constaint logic if source is None
            if source is None:
                continue
            targets = constraint.targets(dr)
            context = "" if type(source) is not list else " when " + source[0].node.info() + " is present"
            if len(targets) != constraint.count():
                dr.interpreter.createErrorDiagnostic(dr.node,
                            "has "+str(len(targets))+" of: ["
                            +", ".join(constraint.targets_predicated())+"] - exactly " + str(constraint.count())+" must occur" + context + "!")

    def _conductAvailableChecks(self, dr:DeserializedResult):
        # Either this is leaf or parent
        # Leaf available checks are value range, type, enumeration, etc.
        try:
            if self._children:
                for key in self._children:
                    self._conductAvailableChildChecks(key, dr)

                # conduct child set checks...

                # conduct uniqueness constraints checks
                self._conductUniquenessChecks(dr)

                # conduct exists constraint checks
                self._conductExistsChecks(dr)

                # conduct at most constraint checks
                self._conductAtMostChecks(dr)

                # conduct at least constraint checks
                self._conductAtLeastChecks(dr)

                # conduct exactly constraint checks
                self._conductExactlyChecks(dr)
            else:
                self._conductChecks(dr)


        except Exception as exception:
            import traceback
            dr.interpreter.createErrorDiagnostic(dr.node, traceback.format_exc())

    def description(self):
        '''The brief description of the input object.
        None if not specified
        '''
        return self._description

    def deserialize(self, node, interpreter):
        '''Deserialize the current node according to this inputObject '''
        thisResult = DeserializedResult(node, interpreter)
        for c in node:
            if self._children is not None and c.name() in self._children:
                childResult = self._children[c.name()].deserialize(c, interpreter)
                thisResult.addResult(childResult, self)
            # 'id', etc. is considered decorative in some langauges but may have constraints
            # so first check if a constraint exists then skip if it is decorative
            elif c.isDecorative(): continue
            else:
                interpreter.createErrorDiagnostic(c, "unknown key!")

        # Set parameters values from user data
        try:
            if self.action():
                self.action()(thisResult)
        except Exception as exception:
            thisResult.interpreter.createErrorDiagnostic(node, str(exception))

        # Conduct set-level diagnostic checks
        self._conductAvailableChecks(thisResult)

        # Set unspecified defaulted parameters
        if self._defaults is not None:
            for key, value in self._defaults.items():
                # If not in the user data it was not specified
                # Force the default value into the user data
                if key not in thisResult.userData:
                    thisResult.userData[key] = DeserializedResult.fromDefault(node, interpreter, value)

        return thisResult

    def _getattr(self, name:str, childKey):
        attr = getattr(self,"_"+name)
        if childKey in attr:
            return attr[childKey]
        return None

    def default(self, childKey):
        return self._getattr('defaults', childKey)

    def enumerations(self):
        return self._enums

    def maxOccurs(self, childKey):
        return self._getattr('maxOccurs', childKey)

    def minOccurs(self, childKey):
        return self._getattr('minOccurs', childKey)

    def minValInc(self, childKey):
        return self._getattr('minValInc', childKey)

    def minValExc(self, childKey):
        return self._getattr('minValExc', childKey)

    def maxValInc(self, childKey):
        return self._getattr('maxValInc', childKey)

    def maxValExc(self, childKey):
        return self._getattr('maxValExc', childKey)

    def serialize(self, io, level=0):

        indent = " "*(level)
        if self.description():
            io.write(indent+"Description='"+self.description()+"'\n")
        if self.enumerations():
            io.write(indent+"ValueEnums["+(" ".join(self._enums))+"]\n")


        if self._children is None:
            return
        for key, child in self._children.items():

            if self.minOccurs(key):
                io.write(indent+"MinOccurs("+key+")="+str(self.minOccurs(key))+"\n")
            if self.maxOccurs(key):
                io.write(indent+"MaxOccurs("+key+")="+str(self.maxOccurs(key))+"\n")
            io.write (" "*level)
            io.write(key+":{\n")
            child.serialize(io, level+2)
            io.write(" "*level)
            io.write("}\n")

    def __getitem__(self, key):
        '''Obtain the named child input object.

        Allows interaction with child input object constraints.
        '''
        if key in self:
            return self._children[key]
        # return null object
        return None

    def __contains__(self, key):
        return self._children is not None and key in self._children

class ExistsConstraint:
    '''Exists Constraint class provides ability to list data sources that must exist
     in a given target context '''
    def __init__(self, source, **kwargs):
        self._source = source # list(str) source data that must exist in given targets or range
        self._target = None # list(str) - target data that must exist if being referenced
        self._discrete = None # list(str) - discrete values
        self._target = kwargs.pop("target", None)
        self._discrete = kwargs.pop("discrete", None)
        assert type(source) is list, "ExistsConstraints source must be a list of source contexts!"
        assert len(source) > 0, "ExistsConstraints source must not be an empty list of source contexts!"
        assert type(self._target) is list, "ExistsConstraints target attribute must be a list of target contexts!"
        assert self._discrete is None or type(self._discrete) is list, "ExistsConstraints discrete attribute must be a list!"
        assert len(self._target) > 0, "ExistsConstraints target attribute must not be an empty list of target contexts!"
        assert len(kwargs) == 0, "ExistsConstraints has unknown keyword arguments! "+str(kwargs)

    def sources(self, queryRoot):
        '''Obtain the set of sources given the queryRoot
            queryRoot:InputObject|DeserializedResult - the point from which to conduct source selection
            returns all existing InputObject or DeserializedResult
        '''
        all_source = []
        for s in self._source:
            selection = queryRoot.select(s)
            if selection is not None:
                all_source.extend(selection)
        return all_source

    def targets(self, queryRoot):
        '''Obtain the set of targets given the queryRoot
            queryRoot:InputObject|DeserializedResult - the point from which to conduct target selection
            returns all existing InputObject or DeserializedResult
        '''
        all_targets = []
        for t in self._target:
            selection = queryRoot.select(t)
            if selection is not None:
                all_targets.extend(selection)
        # Add discrete values
        if self._discrete:
            all_targets.extend(self._discrete)
        return all_targets

class SourcePredicatedTarget:
    '''This class provides data container for contextually predicated target relationship constraints (AtMost, AtLeast, Exactly)
    between data.
        SourcePredicatedTarget(source="child/path"|None, target={"child/id"=[None,"ted", 1]... }, count=1)
        \param source (str) is prerequisite node that must exist for application of target constrain logic
        \param target (dict("path":[None|value1...valuen]))

    '''
    def __init__(self, **kwargs):
        self._source = kwargs.pop("source", None)
        self._target = kwargs.pop("target", {})
        self._count = kwargs.pop("count", 1)
        assert type(self._target) is dict, "SourcePredicateTarget target attribute must be a dict of targets!"
        assert len(self._target) > 0, "SourcePredicateTarget target attribute must not be an empty list of target contexts!"
        assert self._count > 0, "SourcePredicateTarget count must be positive!"
        assert len(kwargs) == 0, "SourcePredicateTarget has unknown keyword arguments! "+str(kwargs)

    def source(self, queryRoot):
        if self._source is not None:
            return queryRoot.select(self._source)
        # source == None is shorthand for self
        return self

    def targets(self, queryRoot):
        '''Obtain the set of targets given the queryRoot
            queryRoot:InputObject|DeserializedResult - the point from which to conduct target selection
            returns all existing InputObject or DeserializedResult

            Returns dictionary of realized targets. I.e., input that has instances
        '''
        all_targets = {}
        for target, predicates in self._target.items():
            selected = queryRoot.select(target)
            if selected and len(selected) > 0:
                pred_list = True if isinstance(predicates, list) else False
                # Reserved term of None indicates any node
                none_predicate = None in predicates if pred_list else predicates is None
                if (pred_list and None in predicates) or (not pred_list and predicates is None):
                    all_targets[target] = None
                for s in selected:
                    sr = s.storedResult()
                    if sr is not None:
                        target_match = False
                        if pred_list and str(sr).lower() in (str(pval).lower() for pval in predicates):
                            target_match = True
                        elif not pred_list and str(sr).lower() == str(predicates).lower():
                            target_match = True
                        if target_match:
                            all_targets[target+"="+str(s.storedResult())] = s
        return all_targets

    def targets_predicated(self):
        '''Obtain the list of targets with assigned predicates
        /path/to/target, /path/to/target=foo
        '''
        strs = []
        for target, values in self._target.items():
            if not isinstance(values, list):
                if values is None: strs.append(target)
                else:              strs.append(target+"="+str(values))
                continue
            for v in values:
                if v is None:
                    strs.append(target)
                else:
                    strs.append(target+"="+str(v))
        return strs
    def count(self):
        return self._count

def storeInt(result):
    try: result.store(int(str(result.node)))
    except: result.interpreter.createErrorDiagnostic(result.node,
                "'" + str(result.node) + "' is expected to be an integer!")

def storeFloat(result):
    try: result.store(float(str(result.node)))
    except: result.interpreter.createErrorDiagnostic(result.node,
                "'" + str(result.node) + "' is expected to be a float!")

def storeStr(result):
    result.store(str(result.node))



