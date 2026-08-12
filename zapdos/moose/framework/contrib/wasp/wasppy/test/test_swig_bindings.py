import unittest
from wasp import *
from Database import InputObject, storeFloat, storeInt, storeStr, ExistsConstraint, SourcePredicatedTarget
from sch2db import write_database
from io import StringIO
from contextlib import redirect_stdout
import math

class LinearModel:
    Definition = None
    @staticmethod
    def definition():
        if LinearModel.Definition is not None: return LinearModel.Definition
        dens = InputObject(Desc="Salt density")
        dens.createRequiredSingle("A", Desc="Density A Coefficient").createRequiredSingle("value", Action=storeFloat)
        dens.createRequiredSingle("B", Desc="Density B Coefficient").createRequiredSingle("value", Action=storeFloat)
        dens.createSingle("C", Default=1.0, Desc="Density C Coefficient").createRequiredSingle("value", Action=storeFloat)
        dens.createRequiredSingle("MinTemp", Desc="Minimum temperature").createRequiredSingle("value", MinValExc=0, Action=storeFloat)
        # TODO add max temperature MinValExc dependency support
        dens.createRequiredSingle("MaxTemp", Desc="Maximum temperature").createRequiredSingle("value", Action=storeFloat)
        dens.createSingle("Type", Default="linear", Desc="interpolation type").createRequiredSingle("value", Enums=["linear"], Action=storeStr)

        LinearModel.Definition = dens
        return LinearModel.Definition

    @staticmethod
    def createFrom(do:'DeserializedObject'):

        result = LinearModel()
        result._a = do["A"].value()
        result._b = do["B"].value()
        result._c = do["C"].value()
        result._minT = do["MinTemp"].value()
        result._maxT = do["MaxTemp"].value() if do["MaxTemp"].hasValue() else result._minT

        theType = do["Type"].value()

        # Require Type to be a supported enumeration
        # This demonstrates post deserialization diagnostic generation
        enumerations = result.definition()["Type"]["value"].enumerations()
        if theType not in enumerations:
            do.interpreter.createWarningDiagnostic(do["Type"].node, "has value of "+str(do["Type"].node)+" which is not listed in "+str(enumerations))

        return result

    ''' _b*x + _c*y = _a '''
    def __init__(self,params=None):
        self._a = 0.0
        self._b = 1.0
        self._c = 1.0
        self._minT = math.inf
        self._maxT = -math.inf

        if params is not None:
            for it in params:
                if it.name() == "MinTemp":
                    self._maxT = float(it)
                elif it.name() == "MaxTemp":
                    self._minT = float(it)
                elif it.name() == "A":
                    self._a = float(it)
                elif it.name() == "B":
                    self._b = float(it)
                elif it.name() == "C":
                    self._c = float(it)

    def get_y(self,x: float) -> "float":
        if self._c == 0.0:
            return math.inf

        return (self._a - (self._b * x)) / self._c
    def __str__(self):
        return "density - a:"+str(self._a)+" b:"+str(self._b)+" c:"+str(self._c)+" min:"+str(self._minT)+" max:"+str(self._maxT)

class Salt:
    Definition = None
    @staticmethod
    def definition():
        '''
            return inputObject - the definition of this object

        '''
        if Salt.Definition is not None: return Salt.Definition
        salt = InputObject(Desc="Single Salt instance")
        salt.createRequiredSingle("id", Enums=["LiF", "NaF", "CaF2", "NH4F", "NaCl"], Desc="Salt type", Action=storeStr)
        salt.createRequiredSingle("BoilTemp", Desc="Salting boiling temperature") \
                .createRequiredSingle("value", MinValExc=0, Action=storeInt)
        salt.createRequiredSingle("MeltTemp", Desc="Salt melting temperature").createRequiredSingle("value", Action=storeFloat)
        salt.createSingle("MolecularWeight", Desc="Salt's molecular weight").createRequiredSingle("value", MinValExc=0, Action=storeFloat)
        salt.addRequiredSingle("Density", LinearModel.definition())


        Salt.Definition = salt
        return Salt.Definition

    @staticmethod
    def createFrom(do:'DeserializedObject'):
        '''
            deserializedObject - Salt object data deserialized from user input
            Create a Salt object from the given data and return it to the caller
        '''

        result = Salt()
        result._name = do["id"] # not an id=value, just salt(id)
        result._molew = do["MolecularWeight"].value() # is a key=value MolecularWeight=value
        result._meltT = do["MeltTemp"].value()

        # In case the value of BoilTemp is bad (i.e., ignored deserialization error diagnostics)
        result._boilT = do["BoilTemp"].value() if do["BoilTemp"].hasValue() else 0

        result._density = LinearModel.createFrom(do["Density"])

        return result

    def __init__(self,params=None):
        self._name = ""
        self._molew = 0.0
        self._meltT = 0.0
        self._boilT = 0.0
        self._density: LinearModel

        # Process parse tree parameters if provided
        if params is not None:
            for it in params:
                if it.name() == "id":
                    self._name = str(it)
                elif it.name() == "MolecularWeight":
                    self._molew = float(it)
                elif it.name() == "MeltTemp":
                    self._meltT = float(it)
                elif it.name() == "BoilTemp":
                    self._boilT = float(it)
                elif it.name() == "Density":
                    self._density = LinearModel(it)


    def density(self,T: float) -> "float":
        return self._density.get_y(T)

    def __str__(self):
        return "salt id:"+self._name+" mw:"+str(self._molew)+" melt:"+str(self._meltT)+" boil:"+str(self._boilT)+"\n    "+str(self._density)

class TheInput:
    Definition = None
    @staticmethod
    def definition():
        if TheInput.Definition is not None: return TheInput.Definition
        db = InputObject()
        salts = db.createRequiredSingle("salts", Desc="The collection of salts in the system")
        salts.addRequired("salt", Salt.definition())
        salts.addUniqueConstraint(["salt/id"])
        db.createRequiredSingle("queries", Desc="Parameters for queries salt properties") \
            .createRequiredSingle("temperatures", Desc="Temperatures (C) at which to query density") \
                .create("value", MinOccurs=1, MaxOccurs=3, MinValExc=0, Action=storeInt)
        TheInput.Definition = db
        return TheInput.Definition

    def createFrom(do:'DeserializedObject'):

        result = TheInput()
        result.salts = [Salt.createFrom(salt) for salt in do["salts"]["salt"]]
        result.queryTemps = do["queries"]["temperatures"].valuelist()

        return result

    def __init__(self):
        self.salts = None
        self.queryTemps = None

    def __str__(self):
        return "".join(str(x)+"\n" for x in self.salts) + "\nqueried at: "+(",".join(str(t) for t in self.queryTemps))+"\n"

class TestSwigInterface(unittest.TestCase):

    def test_parseString(self):
         self.son = DefaultSONInterpreter()
         son_data = "obj{ c1{} a1{} } k=3.14"
         self.assertTrue(self.son.parseString(son_data))
         self.assertEqual(son_data, self.son.root().data())
         # obj, k
         self.assertEqual(2, self.son.root().child_count())
         expected = {"obj":"obj{ c1{} a1{} }", "k" :"k=3.14"}
         for nv in self.son.root().non_decorative_children():
              self.assertEqual(expected[nv.name()], nv.data())

    def test_ddi(self):
        #son
        self.son = DefaultSONInterpreter()
        self.assertTrue(self.son.parseFile(string("test/DDI2Python_schema.sch")))

        #definition
        ddi_def = Definition()
        #At present we have to get a wrapped std::ostream from the wrapper...
        #It would be nice, to add an interface that would let us use something
        #like StringIO
        psuedo_ss = stringstream()
        self.assertTrue(HIVE.create_definition(ddi_def,self.son.root(),psuedo_ss,False))

        #ddi
        ddi = DefaultDDInterpreter()
        #NOTE: there is some magic here with python to smart pointer
        ddi.set_definition_store(ddi_def)
        self.assertTrue(ddi.parseFile(string("test/DDI2Python_input.ddi")))

    def test_eddi(self):
        #son
        self.son = DefaultSONInterpreter()
        self.assertTrue(self.son.parseFile(string("test/DDI2Python_schema.sch")))

        #definition
        ddi_def = Definition()
        #At present we have to get a wrapped std::ostream from the wrapper...
        #It would be nice, to add an interface that would let us use something
        #like StringIO
        psuedo_ss = stringstream()
        self.assertTrue(HIVE.create_definition(ddi_def,self.son.root(),psuedo_ss,False))

        #ddi
        ddi = DefaultEDDInterpreter()
        #NOTE: there is some magic here with python to smart pointer
        ddi.set_definition_store(ddi_def)
        self.assertTrue(ddi.parseFile(string("test/DDI2Python_input.ddi")))

    def test_API(self):
        doc = Interpreter(Syntax.DDI,
                path="test/DDI2Python_input.ddi",
                schema="test/DDI2Python_schema.sch"
                )
        self.assertEqual(None, doc.errors())
        root = doc.root()
        self.assertTrue(isinstance(root,WaspNode))
        self.assertTrue(isinstance(root[1],WaspNode))
        self.assertTrue(isinstance(root[1]._nodeview,DDINodeView))
        self.assertEqual(root[1]._nodeview.data(),root.extract_from[0]._nodeview.data())
        self.assertEqual(root[2]._nodeview.data(),root.extract_from[1]._nodeview.data())
        self.assertEqual(root["extract_from"][0]._nodeview.data(),root.extract_from[0]._nodeview.data())
        self.assertEqual(int(doc.root().extract_from[1].find[0][3][2]),5)
        self.assertEqual(str(doc.root().extract_from[1].find[0][3]['as']),"response_name_6")
        self.assertEqual(str(doc.root().extract_from[1].find[0][3]['as'].value),"response_name_6")

        i = 0
        for it in root:
            j = 0
            for jt in it:
                if len(jt) > 0:
                    k = 0
                    for kt in jt:
                        if len(kt) > 0:
                            l = 0
                            for lt in kt:
                                self.assertEqual(str(root[i][j][k][l]),str(lt))
                                ref = int(root[i][j][k][l])
                                if not ref == 0:
                                    self.assertEqual(int(root[i][j][k][l]),int(lt))
                                else:
                                    ref = float(root[i][j][k][l])
                                    if not ref == 0.0:
                                        self.assertEqual(float(root[i][j][k][l]),float(lt))
                                l += 1
                        k += 1
                j += 1
            i += 1

        #We are going to get "response_name_4" from the line "column 3 4 delimiter '-' as 'response_name_4'
        for it in root:
            if it.name() == "extract_from" and str(it) == "filename_1.out":
                for jt in it:
                    if jt.name() == "find":
                        for kt in jt:
                            if kt.name() == "column" and int(kt) == 3:
                                for lt in kt:
                                    if lt.name() == "as":
                                        self.assertEqual(str(lt),"response_name_4")

    def test_error(self):
        # This test is just checking that it fails to find a file,
        # a far more robust set could be considered

        doc = Interpreter(Syntax.SON,path="Not_Real.son")
        self.assertFalse(doc) # documents are not valid
        parse_diagnostics = doc.parseDiagnostics()
        self.assertEqual(["Not_Real.son:1.1 is either inaccessible or doesn't exist! Unable to read."], [str(x) for x in parse_diagnostics])


    def test_example(self):
        import tempfile

        doc: Interpreter
        import_name:str
        with tempfile.NamedTemporaryFile(mode='r+',delete=False) as f_i:
            f_i.write('  temperatures = [1100,1200,1300,1400]\n')
            import_name = f_i.name

        with tempfile.NamedTemporaryFile(mode='r+',delete=False) as mainfile:
            mainfile.write('salts {\n')
            mainfile.write('  salt(LiF) {\n')
            mainfile.write('      MeltTemp : 1121.2\n')
            mainfile.write('      MolecularWeight : 25.9394\n')
            mainfile.write('      BoilTemp : 2512\n')
            mainfile.write('      Density\n')
            mainfile.write('      {\n')
            mainfile.write('          Type : linear\n')
            mainfile.write('          A : 2.37\n')
            mainfile.write('          B : 5.0e-4\n')
            mainfile.write('          MinTemp : 1123.6\n')
            mainfile.write('          MaxTemp : 1367.5\n')
            mainfile.write('      }\n')
            mainfile.write('  }\n')
            mainfile.write('  salt(NaF) {\n')
            mainfile.write('      MolecularWeight : 41.9882\n')
            mainfile.write('      MeltTemp : 1268\n')
            mainfile.write('      BoilTemp : 1978.0\n')
            mainfile.write('      Density\n')
            mainfile.write('      {\n')
            mainfile.write('          Type : linear\n')
            mainfile.write('          A : 2.76\n')
            mainfile.write('          B : 6.36e-4\n')
            mainfile.write('          MinTemp : 1273\n')
            mainfile.write('          MaxTemp : 1373\n')
            mainfile.write('      }\n')
            mainfile.write('  }\n')
            mainfile.write('}\n')
            #The import statement
            mainfile.write('queries {\n')
            mainfile.write(f'`import ("{import_name}")')
            #mainfile.write('  temperatures = [1100,1200,1300,1400]\n')
            mainfile.write('  }\n')

        doc = Interpreter(Syntax.SON,path=mainfile.name)

        # Test finding a node given line and column
        node = doc.find(3,7) # |MeltTemp : 1121.2
        self.assertEqual("MeltTemp : 1121.2", node.parent().data())

        temps = []
        salt = []
        for it in doc: #either salts or queries
            for jt in it: # salt or temperatures
                if it.name() == "queries" and jt.name() == "temperatures":
                    for kt in jt:
                        if kt.name() == "value":
                            temps.append(float(kt))
                elif jt.name() == "salt":
                    #The call to iter.__next__() returns a doc node, which is
                    #passed in as params and used to initialize the object
                    salt.append(Salt(jt))

        #Check that 2 Salt objects were set up correctly
        self.assertTrue(len(salt) == 2)
        self.assertEqual(salt[0]._name,"LiF")
        self.assertEqual(salt[0]._meltT,1121.2)
        self.assertEqual(salt[0]._boilT,2512.0)
        self.assertEqual(salt[0]._molew,25.9394)
        self.assertEqual(salt[1]._name,"NaF")
        self.assertEqual(salt[1]._meltT,1268.0)
        self.assertEqual(salt[1]._boilT,1978.0)
        self.assertEqual(salt[1]._molew,41.9882)

        # #Check that a set of queries were read correctly from input
        self.assertTrue(len(temps) == 4)
        self.assertEqual(temps[0],1100.0)
        self.assertEqual(temps[1],1200.0)
        self.assertEqual(temps[2],1300.0)
        self.assertEqual(temps[3],1400.0)

        #Final sanity check that all is well using the two test classes
        self.assertEqual(salt[0].density(1200.0),1.77)
        self.assertEqual(salt[1].density(1200.0),1.9968)

    def test_stream(self):
        import tempfile

        mainfile = tempfile.NamedTemporaryFile(mode='r+',delete=False)
        doc: Interpreter
        ss = '''
        salts {
          salt(LiF) {
              MeltTemp : 1121.2
              MolecularWeight : 25.9394
              BoilTemp : 2512
              Density
              {
                  Type : linear
                  A : 2.37
                  B : 5.0e-4
                  MinTemp : 1123.6
                  MaxTemp : 1367.5
              }
          }
          salt(NaF) {
              MolecularWeight : 41.9882
              MeltTemp : 1268
              BoilTemp : 1978
              Density
              {
                  Type : linear
                  A : 2.76
                  B : 6.36e-4
                  MinTemp : 1273
                  MaxTemp : 1373
              }
          }
        }
        queries {
          temperatures = [1100,1200,1300,1400]
        }'''
        schema='''salts{
    Description = "The collection of salts in the system"
    MinOccurs = 1
    MaxOccurs = 1

    salt{
        MinOccurs = 1
        MaxOccurs = NoLimit
        id{
            MinOccurs = 1
            MaxOccurs = 1
            ValEnums = [LiF NaF CaF2 NH4F NaCl]
        }
        BoilTemp{
            MinOccurs = 1
            MaxOccurs = 1
            value{
                MinOccurs = 1
                MaxOccurs = 1
                ValType   = Real
            } % end value
        } % end BoilTemp

        Density{
            MinOccurs = 1
            MaxOccurs = 1
            A{
                MinOccurs = 1
                MaxOccurs = 1
                value{
                    MinOccurs = 1
                    MaxOccurs = 1
                    ValType   = Real
                } % end value
            } % end A

            B{
                MinOccurs = 1
                MaxOccurs = 1
                value{
                    MinOccurs = 1
                    MaxOccurs = 1
                    ValType   = Real
                    MinValInc = 0
                } % end value
            } % end B
            C{
                MinOccurs = 1
                MaxOccurs = 1
                value{
                    MinOccurs = 1
                    MaxOccurs = 1
                    ValType   = Real
                    MinValInc = 0
                } % end value
            } % end C

            MaxTemp{
                MinOccurs = 1
                MaxOccurs = 1
                value{
                    MinOccurs = 1
                    MaxOccurs = 1
                    ValType   = Real
                    MinValExc = "../../MinTemp/value"
                } % end value
            } % end MaxTemp

            MinTemp{
                MinOccurs = 1
                MaxOccurs = 1
                value{
                    MinOccurs = 1
                    MaxOccurs = 1
                    ValType   = Real
                    MinValExc = 0
                } % end value
            } % end MinTemp

            Type{
                Description = "Type of interpolation"
                MinOccurs = 0
                MaxOccurs = 1
                value{
                    InputDefault = "linear"
                    MinOccurs = 1
                    MaxOccurs = 1
                    ValType   = String
                    ValEnums  = [ "linear" ]
                } % end value
            } % end Type
        } % end Density

        MeltTemp{
            MinOccurs = 1
            MaxOccurs = 1

            value{
                MinOccurs = 1
                MaxOccurs = 1
                ValType   = Real
                MinValInc = 0
            } % end value
        } % end MeltTemp

        MolecularWeight{
            MinOccurs = 0
            MaxOccurs = 1
            InputDefault = "1.0"
            value{
                MinOccurs = 1
                MaxOccurs = 1
                ValType   = Real
                MinValExc = 0
            } % end value
        } % end MolecularWeight
    } % end salt
} % end salts

queries{
    Description = "Parameters for queries salt properties"
    MinOccurs = 1
    MaxOccurs = 1

    temperatures{
        Description = "Temperatures (C) at which to query density"
        MinOccurs = 1
        MaxOccurs = 1
        value{
            MinOccurs = 1
            MaxOccurs = NoLimit
            ValType   = Real
            MinValInc = 0
        } % end value
    } % end temperatures
} % end queries
'''
        doc = Interpreter(Syntax.SON, schema=schema, data=ss, path=mainfile.name)

        temps = []
        salt = []
        for it in doc: #either salts or queries
            for jt in it: # salt or temperatures
                if it.name() == "queries" and jt.name() == "temperatures":
                    for kt in jt:
                        if kt.name() == "value":
                            temps.append(float(kt))
                elif jt.name() == "salt":
                    #The call to iter.__next__() returns a doc node, which is
                    #passed in as params and used to initialize the object
                    salt.append(Salt(jt))

        #Check that 2 Salt objects were set up correctly
        self.assertTrue(len(salt) == 2)
        self.assertEqual(salt[0]._name,"LiF")
        self.assertEqual(salt[0]._meltT,1121.2)
        self.assertEqual(salt[0]._boilT,2512.0)
        self.assertEqual(salt[0]._molew,25.9394)
        self.assertEqual(salt[1]._name,"NaF")
        self.assertEqual(salt[1]._meltT,1268.0)
        self.assertEqual(salt[1]._boilT, 1978)
        self.assertEqual(salt[1]._molew,41.9882)

        # #Check that a set of queries were read correctly from input
        self.assertTrue(len(temps) == 4)
        self.assertEqual(temps[0],1100.0)
        self.assertEqual(temps[1],1200.0)
        self.assertEqual(temps[2],1300.0)
        self.assertEqual(temps[3],1400.0)

        #Final sanity check that all is well using the two test classes
        self.assertEqual(salt[0].density(1200.0),1.77)
        self.assertEqual(salt[1].density(1200.0),1.9968)

        # Test __getattr__ (. operator) through VectorWaspNode
        # Each value belongs to different 'BoilTemp' requiring a VectorWaspNode
        boiltemps = doc.root().salts.salt.BoilTemp.value
        self.assertEqual(2, len(boiltemps))
        self.assertEqual(2512, float(boiltemps[0]))
        self.assertEqual(1978, float(boiltemps[1]))
        self.assertEqual("LiF", str(boiltemps[0].parent().parent().id))
        self.assertEqual("NaF", str(boiltemps[1].parent().parent().id))

    def test_validation_errors(self):
        schema = '''data{MaxOccurs=1 value{MaxOccurs=1 MaxValInc=1.0 ValType=Real}}'''
        input = '''data 1.1'''

        # Test EDDI
        interpreter = Interpreter(Syntax.EDDI, schema=schema, data=input, path="myfile.inp")

        errors = interpreter.errors()
        expected = ["line:1 column:6 - Validation Error: data value \"1.1\" is greater than the allowed maximum inclusive value of 1.0"]
        self.assertEqual(expected, errors)
        # The 'data' is ambiguous between the child 'data' and the node method 'data()'
        # Get the child by using the string-based query bracket operator
        data = interpreter.root()['data']
        self.assertEqual("myfile.inp", data.source())
        self.assertFalse(data.isDecorative())

        # Test DDI
        input = "data 1.3"
        interpreter = Interpreter(Syntax.DDI, schema=schema, data=input, path="myfile.inp")
        errors = interpreter.errors()
        expected = ["line:1 column:6 - Validation Error: data value \"1.3\" is greater than the allowed maximum inclusive value of 1.0"]
        self.assertEqual(expected, errors)

        # Test SON
        input = "data=1.8"
        interpreter = Interpreter(Syntax.SON, schema=schema, data=input, path="myfile.inp")
        errors = interpreter.errors()
        expected = ["line:1 column:6 - Validation Error: data value \"1.8\" is greater than the allowed maximum inclusive value of 1.0"]
        self.assertEqual(expected, errors)

        # Test HIT
        input = "data=1.7"
        interpreter = Interpreter(Syntax.HIT, schema=schema, data=input, path="myfile.inp")
        errors = interpreter.errors()
        expected = ["line:1 column:6 - Validation Error: data value \"1.7\" is greater than the allowed maximum inclusive value of 1.0"]
        self.assertEqual(expected, errors)

    def test_parse_diagnostics(self):
        input = 'data =' # missing value

        # Test HIT parse error
        interpreter = Interpreter(Syntax.HIT, data=input, path="myfile.inp")
        self.assertFalse(interpreter)
        errors = interpreter.parseDiagnostics()
        expected = [
            "myfile.inp:1.7: syntax error, unexpected end of file",
            "myfile.inp:1.6: syntax error, 'data' has a missing or malformed value",
        ]

        error_strings = [str(x).rstrip() for x in errors]
        self.assertEqual(expected, error_strings)

    def test_diagnostics(self):
        input = "key=1.8"
        interpreter = Interpreter(Syntax.SON, data=input, path="myfile.inp")
        self.assertTrue(interpreter)
        node = interpreter.root().key[0]['value'][0]
        interpreter.createErrorDiagnostic(node, "is an unexpected parameter!")
        interpreter.createWarningDiagnostic(node, "is an unexpected parameter!")
        interpreter.createInformationDiagnostic(node, "is skipped as an unexpected parameter!")
        interpreter.createHintDiagnostic(node, "should be named less ambiguously!")
        dd = interpreter.deserializeDiagnostics()
        self.assertEqual(4, len(dd))

        expected = ["myfile.inp:1.5: value is an unexpected parameter!",
                    "myfile.inp:1.5: value is an unexpected parameter!",
                    "myfile.inp:1.5: value is skipped as an unexpected parameter!",
                    "myfile.inp:1.5: value should be named less ambiguously!"]
        for i, e in enumerate(expected):
            self.assertEqual(e, str(dd[i]))

    def test_database_inputobject(self):
        '''This test exercises the input object definition creation, deserialization, selection, and diagnostics
        as used by the toy salts classes above
        '''
        ss = '''
        salts {
          salt(LiF) {
              MeltTemp : 1121.2
              MolecularWeight : 25.9394
              BoilTemp : 0
              Density
              {
                  Type : foo
                  A : 2.37
                  B : 5.0e-4
                  MinTemp : 1123.6
                  MaxTemp : 1367.5
              }
          }
          salt(NaF) {
              MolecularWeight : 41.9882
              MeltTemp : 1268
              BoilTemp : 1978
              Density
              {
                  Type : linear
                  A : 2.76
                  B : 6.36e-4
                  MinTemp : 1273
                  MaxTemp : 1373
              }
          }
          salt(NaF){
              MolecularWeight : 41.9882
              MeltTemp : 1268
              BoilTemp : bam
              Density
              {
                  Type : linear
                  A : 2.76
                  B : 6.36e-4
                  MinTemp : 1273
                  MaxTemp : hello
              }
            Density
              {
                  Type : linear
                  A : 2.76
                  B : 6.36e-4
                  MinTemp : 1273
                  MaxTemp : foo
              }
          }
        }
        queries {
          temperatures = [1100,-1200,1300.7,1400]
        }'''

        interpreter = Interpreter(Syntax.SON, path="input.son", data=ss)

        self.assertTrue(interpreter)

        definition = TheInput.definition()
        # Test input object (definition) selection logic
        self.assertEqual(definition['salts']['salt']['MolecularWeight']['value'], definition.select("salts/salt/MolecularWeight/value")[0])
        self.assertEqual(definition['salts']['salt']['MolecularWeight']['value'], definition.select("*/*/MolecularWeight/value")[0])
        self.assertEqual(definition['salts']['salt']['MolecularWeight']['value'], definition.select("*/*/MolecularWeight/*")[0])
        self.assertTrue(definition)

        db = definition.deserialize(interpreter.root(), interpreter)
        self.assertTrue(interpreter.deserializeDiagnostics())
        expectedDiagnostics = '''input.son:6.26: value 0 is less than or equal to the allowed minimum exclusive value of 0!
input.son:9.26: value foo is not one of the allowed values ['linear']!
input.son:32.26: value 'bam' is expected to be an integer!
input.son:32.15: BoilTemp has 0 occurrences of value when 1 are required!
input.son:39.29: value 'hello' is expected to be a float!
input.son:39.19: MaxTemp has 0 occurrences of value when 1 are required!
input.son:47.29: value 'foo' is expected to be a float!
input.son:47.19: MaxTemp has 0 occurrences of value when 1 are required!
input.son:41.13-48.0: Density occurrence exceeds maximum allowed occurrence of 1!
input.son:16.16: id NaF must be unique but is duplicate to id on line 29 column 16
input.son:29.16: id NaF must be unique but is duplicate to id on line 16 column 16
input.son:52.32: value -1200 is less than or equal to the allowed minimum exclusive value of 0!
input.son:52.38: value '1300.7' is expected to be an integer!
input.son:52.45: value has 4 occurrences which exceeds max occurs of 3!
'''
        self.maxDiff = None
        self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))

        # Test deserialized result selection
        saltids = db.select("salts/salt/id")
        self.assertEqual(3, len(saltids))
        self.assertEqual("LiF", saltids[0].storedResult())
        self.assertEqual("NaF", saltids[1].storedResult())
        self.assertEqual("NaF", saltids[2].storedResult())

        self.assertTrue(db["salts"])
        self.assertTrue(type(db["salts"]["salt"]) == list)
        self.assertEqual(3, len(db["salts"]["salt"]))
        self.assertTrue(db["salts"]["salt"][1])
        self.assertTrue(db["salts"]["salt"][1]["MolecularWeight"])
        self.assertEqual(41.9882, db["salts"]["salt"][1]["MolecularWeight"].value())

        # Test defaulted 'C' and given A density coefficient
        self.assertTrue(db["salts"]["salt"][1]["Density"])
        self.assertTrue(db["salts"]["salt"][1]["Density"]["A"])
        self.assertEqual(2.76, db["salts"]["salt"][1]["Density"]["A"].value())
        self.assertTrue(db["salts"]["salt"][1]["Density"]["C"])
        self.assertEqual(1.0, db["salts"]["salt"][1]["Density"]["C"].value())

        self.assertTrue(db["queries"])
        self.assertEqual([1100,-1200,None,1400], db["queries"]["temperatures"].valuelist())

        # create program data structure
        theInput = TheInput.createFrom(db)
        # This test illustrates the additional diagnostics from result to object creation
        expectedDiagnostics = '''input.son:6.26: value 0 is less than or equal to the allowed minimum exclusive value of 0!
input.son:9.26: value foo is not one of the allowed values ['linear']!
input.son:32.26: value 'bam' is expected to be an integer!
input.son:32.15: BoilTemp has 0 occurrences of value when 1 are required!
input.son:39.29: value 'hello' is expected to be a float!
input.son:39.19: MaxTemp has 0 occurrences of value when 1 are required!
input.son:47.29: value 'foo' is expected to be a float!
input.son:47.19: MaxTemp has 0 occurrences of value when 1 are required!
input.son:41.13-48.0: Density occurrence exceeds maximum allowed occurrence of 1!
input.son:16.16: id NaF must be unique but is duplicate to id on line 29 column 16
input.son:29.16: id NaF must be unique but is duplicate to id on line 16 column 16
input.son:52.32: value -1200 is less than or equal to the allowed minimum exclusive value of 0!
input.son:52.38: value '1300.7' is expected to be an integer!
input.son:52.45: value has 4 occurrences which exceeds max occurs of 3!
input.son:9.19: Type has value of foo which is not listed in ['linear']
'''
        self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        self.assertEqual([1100,-1200,None,1400],theInput.queryTemps)
        self.assertEqual(3, len(theInput.salts))

        # Test theInput for deserialized result
        expectedSummary = '''salt id:LiF mw:25.9394 melt:1121.2 boil:0
    density - a:2.37 b:0.0005 c:1.0 min:1123.6 max:1367.5
salt id:NaF mw:41.9882 melt:1268.0 boil:1978
    density - a:2.76 b:0.000636 c:1.0 min:1273.0 max:1373.0
salt id:NaF mw:41.9882 melt:1268.0 boil:0
    density - a:2.76 b:0.000636 c:1.0 min:1273.0 max:1273.0

queried at: 1100,-1200,None,1400
'''
        self.assertEqual(expectedSummary, str(theInput))

    def test_database_inputobject_exists(self):
        definition = InputObject()
        definition.createRequiredSingle("ids", Desc="List of viable identifers").createRequired("value", Action=storeStr)
        definition.createRequired("ref", Desc="The id reference").createRequiredSingle("value", Action=storeStr)
        definition.addExistsConstraint(ExistsConstraint(["ref/value"], target=["ids/value"], discrete=["bob", "robert"]))

        # Add constraint to input that will not be exercised/exists
        definition.createSingle("ref_02", Desc="Another id reference").createRequiredSingle("value", Action=storeStr)
        definition.addExistsConstraint(ExistsConstraint(["ref_02/value"], target=["ids/value"]))

        class obj_with_pass_defaults:
            def definition(self):
                definition = InputObject()
                definition.create("param_pass_enums", Default="red").createSingle("value", Enums=["red", "blue"], Action=storeStr)
                definition.create("param_pass_exist", Default="ted").createSingle("value", Action=storeStr)
                return definition
        class obj_with_fail_defaults:
            def definition(self):
                definition = InputObject()
                definition.create("param_fail_enums", Default="bad").createSingle("value", Enums=["red", "blue"], Action=storeStr)
                definition.create("param_fail_exist", Default="abc").createSingle("value", Action=storeStr)
                definition.create("param_fail_unque", Default="xyz").createSingle("value", Action=storeStr)
                return definition
        definition.add("obj_with_pass_defaults", obj_with_pass_defaults().definition())
        definition.add("obj_with_fail_defaults", obj_with_fail_defaults().definition())
        definition.addExistsConstraint(ExistsConstraint(["obj_with_pass_defaults/param_pass_exist/value"], target=["ids/value"]))
        definition.addExistsConstraint(ExistsConstraint(["obj_with_fail_defaults/param_fail_exist/value"], target=["ids/value"]))
        definition.addUniqueConstraint(["obj_with_fail_defaults/param_fail_unque/value"])

        self.assertEqual(["ids/value"], definition.getExistsConstraintTargetGivenSource("obj_with_pass_defaults/param_pass_exist/value"))
        self.assertEqual(["ref/value","ref_02/value","obj_with_pass_defaults/param_pass_exist/value","obj_with_fail_defaults/param_fail_exist/value"], definition.getExistsConstraintSourceGivenTarget("ids/value"))

        inputcontents = '''ids=[foo bar ted]
        ref=bart
        ref=bob
        ref=foo
        ref=ro
        obj_with_pass_defaults{
            % this object has parameters with
            % default values which would pass
            % both enums and existence checks
        }
        obj_with_fail_defaults{
            % this object has parameters with
            % default values which would fail
            % both enums and existence checks

            % the enums check passes with the
            % default but the existence check
            % fails with an odd error message

            % there is another parameter here
            % failing its unique check due to
            % having a default in two objects
        }
        obj_with_fail_defaults{
            % this object has parameters with
            % default values which would fail
            % both enums and existence checks

            % the enums check passes with the
            % default but the existence check
            % fails with an odd error message

            % there is another parameter here
            % failing its unique check due to
            % having a default in two objects
        }
        '''
        interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

        self.assertTrue(interpreter)

        db = definition.deserialize(interpreter.root(), interpreter)
        self.assertTrue(interpreter.deserializeDiagnostics())
        expectedDiagnostics = '''input.son:11.9-23.0: obj_with_fail_defaults xyz must be unique but is duplicate to obj_with_fail_defaults on line 24 column 9
input.son:24.9-36.0: obj_with_fail_defaults xyz must be unique but is duplicate to obj_with_fail_defaults on line 11 column 9
input.son:2.13: value bart is not one of the required existing targets! Required existing targets are foo, bar, ted, bob, robert!
input.son:5.13: value ro is not one of the required existing targets! Required existing targets are foo, bar, ted, bob, robert!
input.son:11.9-23.0: obj_with_fail_defaults abc is not one of the required existing targets! Required existing targets are foo, bar, ted!
input.son:24.9-36.0: obj_with_fail_defaults abc is not one of the required existing targets! Required existing targets are foo, bar, ted!
'''
        self.maxDiff = None
        self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))

    def test_database_default_list(self):
        definition = InputObject()

        definition.createRequiredSingle("data").createSingle("ids", Desc="List of integers", Default=[1,2,3]).createRequired("value", Action=storeInt)
        def defaulted_list_using_default():
            # Empty string to test default data value
            inputcontents = "data{}"
            interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

            self.assertTrue(interpreter)
            db = definition.deserialize(interpreter.root(), interpreter)
            self.assertFalse(interpreter.deserializeDiagnostics())
            self.assertEqual([1,2,3], db["data"]["ids"].valuelist())
        defaulted_list_using_default()

        def defaulted_list_not_using_default():
            inputcontents = "data{ids=[9 10 11]}"
            interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

            self.assertTrue(interpreter)
            db = definition.deserialize(interpreter.root(), interpreter)
            self.assertFalse(interpreter.deserializeDiagnostics())
            self.assertEqual([9,10, 11], db["data"]["ids"].valuelist())
        defaulted_list_not_using_default()

    def test_database_AtMostConstraint(self):
        definition = InputObject()
        x= definition.create("x")
        x.createSingle("id", Action=storeStr)
        x.create("foo").createRequiredSingle("value", Action=storeInt)
        x.create("bar").createRequiredSingle("value", Action=storeInt)
        x.addAtMostConstraint(SourcePredicatedTarget(target={"bar/value":[3,7], "foo/value":3}))
        definition.create("y").createSingle("id", Action=storeStr)
        definition.create("z").createSingle("id", Action=storeStr)

        definition.addAtMostConstraint(SourcePredicatedTarget(target={"x":None, "y":[None]}))
        definition.addAtMostConstraint(SourcePredicatedTarget(target={"x/id":['foo'],"y/id":'bar'}))
        definition.addAtMostConstraint(SourcePredicatedTarget(source="x/id", target={"z/id":'foo',"y/id":['bar']}))

        # x.id value cannot be 'illegal' or 'invalid', i.e., ChildAtMostOne=[ "id" "id"=illegal "id"=invalid ]
        x.addAtMostConstraint(SourcePredicatedTarget(target={"id":[None, "illegal", "invalid"]}))

        def atmost_component():
            inputcontents = '''
            x { }
            y { }
            '''
            interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

            self.assertTrue(interpreter)

            db = definition.deserialize(interpreter.root(), interpreter)
            self.assertTrue(interpreter.deserializeDiagnostics())
            expectedDiagnostics = '''input.son:2.13-3.0: document has more than 1 of: [x, y] - at most 1 must occur!
'''
            self.maxDiff = None
            self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        atmost_component()

        def atmost_cross_component_id():
            inputcontents = '''
            x (foo){ }
            y (bAr){ }
            z (foo){ }
            '''
            interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

            self.assertTrue(interpreter)

            db = definition.deserialize(interpreter.root(), interpreter)
            self.assertTrue(interpreter.deserializeDiagnostics())
            expectedDiagnostics = '''input.son:2.13-4.0: document has more than 1 of: [x, y] - at most 1 must occur!
input.son:2.13-4.0: document has more than 1 of: [x/id=foo, y/id=bAr] - at most 1 must occur!
input.son:2.13-4.0: document has more than 1 of: [z/id=foo, y/id=bAr] - at most 1 must occur when id on line 2 column 16 is present!
'''
            self.maxDiff = None
            self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        atmost_cross_component_id()

        def atmost_value():
            inputcontents = '''
            x {foo=3 bar=7 bar=3}
            '''
            interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

            self.assertTrue(interpreter)

            db = definition.deserialize(interpreter.root(), interpreter)
            self.assertTrue(interpreter.deserializeDiagnostics())
            expectedDiagnostics = '''input.son:2.13: x has more than 1 of: [bar/value=7, bar/value=3, foo/value=3] - at most 1 must occur!
'''
            self.maxDiff = None
            self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        atmost_value()

        def atmost_exclusion():
            inputcontents = '''
            x           { }
            x (illegal) { }
            x (allowed) { }
            x (invalid) { }
            '''
            interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

            self.assertTrue(interpreter)

            db = definition.deserialize(interpreter.root(), interpreter)
            self.assertTrue(interpreter.deserializeDiagnostics())
            expectedDiagnostics = '''input.son:3.13: x has more than 1 of: [id, id=illegal] - at most 1 must occur!
input.son:5.13: x has more than 1 of: [id, id=invalid] - at most 1 must occur!
'''
            self.maxDiff = None
            self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        atmost_exclusion()

    def test_database_AtLeastConstraint(self):
        definition = InputObject()
        x= definition.create("x")
        x.createSingle("id", Action=storeStr)
        x.create("foo").createRequiredSingle("value", Action=storeInt)
        x.create("bar").createRequiredSingle("value", Action=storeInt)
        x.addAtLeastConstraint(SourcePredicatedTarget(target={"bar/value":[3,7], "foo/value":3}))
        definition.create("y").createSingle("id", Action=storeStr)
        definition.create("z").createSingle("id", Action=storeStr)

        definition.addAtLeastConstraint(SourcePredicatedTarget(target={"x":None, "y":[None]}))
        definition.addAtLeastConstraint(SourcePredicatedTarget(source="z/id", target={"x/id":["bar"], "y/id":"foo"}))
        def atleast_component():
            inputcontents = '''
                z{}
            '''
            interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

            self.assertTrue(interpreter)

            db = definition.deserialize(interpreter.root(), interpreter)
            self.assertTrue(interpreter.deserializeDiagnostics())
            expectedDiagnostics = '''input.son:2.17: document has fewer than 1 of: [x, y] - at least 1 must occur!
'''
            self.maxDiff = None
            self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        atleast_component()
        def atleast_sourced_component():
            inputcontents = '''
                y{}
                x{}
                z(id){} % requires bar id'd x or foo id'd y to be present
            '''
            interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

            self.assertTrue(interpreter)

            db = definition.deserialize(interpreter.root(), interpreter)
            self.assertTrue(interpreter.deserializeDiagnostics())
            expectedDiagnostics = '''input.son:3.17: x has fewer than 1 of: [bar/value=3, bar/value=7, foo/value=3] - at least 1 must occur!
input.son:2.17-4.0: document has fewer than 1 of: [x/id=bar, y/id=foo] - at least 1 must occur when id on line 4 column 19 is present!
'''
            self.maxDiff = None
            self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        atleast_sourced_component()

    def test_database_ExactlyConstraint(self):
        definition = InputObject()
        x= definition.create("x")
        x.createSingle("id", Action=storeStr)
        x.create("foo").createRequiredSingle("value", Action=storeInt)
        x.create("bar").createRequiredSingle("value", Action=storeInt)
        x.addExactlyConstraint(SourcePredicatedTarget(target={"bar/value":3, "foo/value":[3,7]}))
        definition.create("y").createSingle("id", Action=storeStr)
        definition.create("z").createSingle("id", Action=storeStr)

        definition.addExactlyConstraint(SourcePredicatedTarget(target={"x":[None], "y":None}))
        definition.addExactlyConstraint(SourcePredicatedTarget(source="z/id", target={"x/id":"bar", "y/id":["foo"]}))
        def exactly_component():
            inputcontents = '''
                z{}
            '''
            interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

            self.assertTrue(interpreter)

            db = definition.deserialize(interpreter.root(), interpreter)
            self.assertTrue(interpreter.deserializeDiagnostics())
            expectedDiagnostics = '''input.son:2.17: document has 0 of: [x, y] - exactly 1 must occur!
'''
            self.maxDiff = None
            self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        exactly_component()
        def exactly_sourced_component():
            inputcontents = '''
                y{}
                x{}
                z(id){} % requires bar id'd x or foo id'd y to be present
            '''
            interpreter = Interpreter(Syntax.SON, path="input.son", data=inputcontents)

            self.assertTrue(interpreter)

            db = definition.deserialize(interpreter.root(), interpreter)
            self.assertTrue(interpreter.deserializeDiagnostics())
            expectedDiagnostics = '''input.son:3.17: x has 0 of: [bar/value=3, foo/value=3, foo/value=7] - exactly 1 must occur!
input.son:2.17-4.0: document has 2 of: [x, y] - exactly 1 must occur!
input.son:2.17-4.0: document has 0 of: [x/id=bar, y/id=foo] - exactly 1 must occur when id on line 4 column 19 is present!
'''
            self.maxDiff = None
            self.assertEqual(expectedDiagnostics,"".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        exactly_sourced_component()

    def test_node_set_data(self):
        input = 'data = 1.24'

        interpreter = Interpreter(Syntax.SON, data=input, path="myfile.inp")
        self.assertTrue(interpreter)
        self.assertEqual("data = 1.24", interpreter.root().data())
        data_value_node = interpreter.root()['data']['value'][0]
        self.assertTrue(data_value_node)
        data_value_node.set_data("bob")
        self.assertEqual("data = bob", interpreter.root()['data'][0].data())

    def test_sch2db(self):
        self.assertTrue(schema := Interpreter(Syntax.SON, path="test/schema.sch"))
        dbpy_captured = StringIO()
        with redirect_stdout(dbpy_captured):
            write_database(schema.root())
        with open("test/db_expected.py") as dbpy_file:
            dbpy_expected = dbpy_file.read()
        self.maxDiff = None
        self.assertEqual(dbpy_expected, dbpy_captured.getvalue())

if __name__ == '__main__':
     unittest.main()
