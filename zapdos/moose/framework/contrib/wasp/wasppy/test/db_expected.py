from wasp import *
from Database import *

class _pytest:
    def __init__(self, params=None):
        self._definition = None
        self.object_one = None
        self.object_two = None
    def definition(self):
        if self._definition is not None: return self._definition
        _obj = InputObject()
        _obj.add("object_one", self._object_one().definition(), MinOccurs=1, MaxOccurs=1)
        _obj.add("object_two", self._object_two().definition(), MinOccurs=0, MaxOccurs=1)
        _obj.addExistsConstraint(ExistsConstraint( \
            [ "object_two/square/pentagon/foo_name_choices/value" ], \
            target=[ "object_one/foos/foo/id" ]))
        self._definition = _obj
        return self._definition
    def createFrom(self, dr:DeserializedResult):
        result = self.__class__()
        if len(dr["object_one"]) > 0:
            result.object_one = self._object_one().createFrom(dr["object_one"])
        if len(dr["object_two"]) > 0:
            result.object_two = self._object_two().createFrom(dr["object_two"])
        return result

    class _object_one:
        def __init__(self, params=None):
            self._definition = None
            self.color = None
            self.foos = None
            self.bars = None
        def definition(self):
            if self._definition is not None: return self._definition
            _obj = InputObject()
            _obj.create("color", MinOccurs=1, MaxOccurs=1) \
                .create("value", Enums=["red", "blue", "green", "orange"], Action=storeStr)
            _obj.add("foos", self._foos().definition(), MinOccurs=1, MaxOccurs=1)
            _obj.add("bars", self._bars().definition(), MinOccurs=0, MaxOccurs=1)
            self._definition = _obj
            return self._definition
        def createFrom(self, dr:DeserializedResult):
            result = self.__class__()
            if len(dr["color"]) > 0:
                result.color = dr["color"].valuelist()
            if len(dr["foos"]) > 0:
                result.foos = self._foos().createFrom(dr["foos"])
            if len(dr["bars"]) > 0:
                result.bars = self._bars().createFrom(dr["bars"])
            return result

        class _foos:
            def __init__(self, params=None):
                self._definition = None
                self.foo = None
            def definition(self):
                if self._definition is not None: return self._definition
                _obj = InputObject()
                _obj.add("foo", self._foo().definition(), MinOccurs=1)
                self._definition = _obj
                return self._definition
            def createFrom(self, dr:DeserializedResult):
                result = self.__class__()
                if len(dr["foo"]) > 0:
                    result.foo = [self._foo().createFrom(it) for it in dr["foo"]]
                return result

            class _foo:
                def __init__(self, params=None):
                    self._definition = None
                    self.id = None
                    self.numval = None
                    self.aaas = None
                    self.bbbs = None
                    self.cccs = None
                    self.ddds = None
                    self.eees = None
                    self.fffs = None
                def definition(self):
                    if self._definition is not None: return self._definition
                    _obj = InputObject()
                    _obj.create("id", MinOccurs=1, MaxOccurs=1, Action=storeStr)
                    _obj.create("numval", MinOccurs=1, MaxOccurs=1) \
                        .create("value", MinValInc=0, Action=storeFloat)
                    _obj.add("aaas", self._aaas().definition(), MaxOccurs=1)
                    _obj.add("bbbs", self._bbbs().definition(), MaxOccurs=1)
                    _obj.add("cccs", self._cccs().definition(), MaxOccurs=1)
                    _obj.add("ddds", self._ddds().definition(), MaxOccurs=1)
                    _obj.add("eees", self._eees().definition())
                    _obj.add("fffs", self._fffs().definition())
                    _obj.addExactlyConstraint(SourcePredicatedTarget( \
                        target={ "aaas":None, \
                                 "bbbs":None, \
                                 "cccs":None, \
                                 "ddds":None, \
                                 "eees":None, \
                                 "fffs":None }))
                    self._definition = _obj
                    return self._definition
                def createFrom(self, dr:DeserializedResult):
                    result = self.__class__()
                    if dr["id"] is not None:
                        result.id = dr["id"]
                    if len(dr["numval"]) > 0:
                        result.numval = dr["numval"].valuelist()
                    if len(dr["aaas"]) > 0:
                        result.aaas = self._aaas().createFrom(dr["aaas"])
                    if len(dr["bbbs"]) > 0:
                        result.bbbs = self._bbbs().createFrom(dr["bbbs"])
                    if len(dr["cccs"]) > 0:
                        result.cccs = self._cccs().createFrom(dr["cccs"])
                    if len(dr["ddds"]) > 0:
                        result.ddds = self._ddds().createFrom(dr["ddds"])
                    if len(dr["eees"]) > 0:
                        result.eees = [self._eees().createFrom(it) for it in dr["eees"]]
                    if len(dr["fffs"]) > 0:
                        result.fffs = [self._fffs().createFrom(it) for it in dr["fffs"]]
                    return result

                class _aaas:
                    def __init__(self, params=None):
                        self._definition = None
                        self.a_num = None
                    def definition(self):
                        if self._definition is not None: return self._definition
                        _obj = InputObject()
                        _obj.add("a_num", self._a_num().definition(), MinOccurs=1)
                        self._definition = _obj
                        return self._definition
                    def createFrom(self, dr:DeserializedResult):
                        result = self.__class__()
                        if len(dr["a_num"]) > 0:
                            result.a_num = [self._a_num().createFrom(it) for it in dr["a_num"]]
                        return result

                    class _a_num:
                        def __init__(self, params=None):
                            self._definition = None
                            self.id = None
                            self.value = None
                        def definition(self):
                            if self._definition is not None: return self._definition
                            _obj = InputObject()
                            _obj.create("id", MinOccurs=1, Enums=[*EnumList], Action=storeStr)
                            _obj.create("value", MinValInc=0, MaxValInc=1, Action=storeFloat)
                            self._definition = _obj
                            return self._definition
                        def createFrom(self, dr:DeserializedResult):
                            result = self.__class__()
                            if dr["id"] is not None:
                                result.id = dr["id"]
                            if len(dr["value"]) > 0:
                                result.value = [it.storedResult() for it in dr["value"]]
                            return result

                class _bbbs:
                    def __init__(self, params=None):
                        self._definition = None
                        self.b_num = None
                    def definition(self):
                        if self._definition is not None: return self._definition
                        _obj = InputObject()
                        _obj.add("b_num", self._b_num().definition(), MinOccurs=1)
                        self._definition = _obj
                        return self._definition
                    def createFrom(self, dr:DeserializedResult):
                        result = self.__class__()
                        if len(dr["b_num"]) > 0:
                            result.b_num = [self._b_num().createFrom(it) for it in dr["b_num"]]
                        return result

                    class _b_num:
                        def __init__(self, params=None):
                            self._definition = None
                            self.id = None
                            self.value = None
                        def definition(self):
                            if self._definition is not None: return self._definition
                            _obj = InputObject()
                            _obj.create("id", MinOccurs=1, Enums=[*EnumList], Action=storeStr)
                            _obj.create("value", MinValInc=0, Action=storeFloat)
                            self._definition = _obj
                            return self._definition
                        def createFrom(self, dr:DeserializedResult):
                            result = self.__class__()
                            if dr["id"] is not None:
                                result.id = dr["id"]
                            if len(dr["value"]) > 0:
                                result.value = [it.storedResult() for it in dr["value"]]
                            return result

                class _cccs:
                    def __init__(self, params=None):
                        self._definition = None
                        self.c_num = None
                    def definition(self):
                        if self._definition is not None: return self._definition
                        _obj = InputObject()
                        _obj.add("c_num", self._c_num().definition(), MinOccurs=1)
                        self._definition = _obj
                        return self._definition
                    def createFrom(self, dr:DeserializedResult):
                        result = self.__class__()
                        if len(dr["c_num"]) > 0:
                            result.c_num = [self._c_num().createFrom(it) for it in dr["c_num"]]
                        return result

                    class _c_num:
                        def __init__(self, params=None):
                            self._definition = None
                            self.id = None
                            self.value = None
                        def definition(self):
                            if self._definition is not None: return self._definition
                            _obj = InputObject()
                            _obj.create("id", MinOccurs=1, Enums=[*EnumList], Action=storeStr)
                            _obj.create("value", MinValInc=0, MaxValInc=1, Action=storeFloat)
                            self._definition = _obj
                            return self._definition
                        def createFrom(self, dr:DeserializedResult):
                            result = self.__class__()
                            if dr["id"] is not None:
                                result.id = dr["id"]
                            if len(dr["value"]) > 0:
                                result.value = [it.storedResult() for it in dr["value"]]
                            return result

                class _ddds:
                    def __init__(self, params=None):
                        self._definition = None
                        self.d_num = None
                    def definition(self):
                        if self._definition is not None: return self._definition
                        _obj = InputObject()
                        _obj.add("d_num", self._d_num().definition(), MinOccurs=1)
                        self._definition = _obj
                        return self._definition
                    def createFrom(self, dr:DeserializedResult):
                        result = self.__class__()
                        if len(dr["d_num"]) > 0:
                            result.d_num = [self._d_num().createFrom(it) for it in dr["d_num"]]
                        return result

                    class _d_num:
                        def __init__(self, params=None):
                            self._definition = None
                            self.id = None
                            self.value = None
                        def definition(self):
                            if self._definition is not None: return self._definition
                            _obj = InputObject()
                            _obj.create("id", MinOccurs=1, Enums=[*EnumList], Action=storeStr)
                            _obj.create("value", MinValInc=0, Action=storeFloat)
                            self._definition = _obj
                            return self._definition
                        def createFrom(self, dr:DeserializedResult):
                            result = self.__class__()
                            if dr["id"] is not None:
                                result.id = dr["id"]
                            if len(dr["value"]) > 0:
                                result.value = [it.storedResult() for it in dr["value"]]
                            return result

                class _eees:
                    def __init__(self, params=None):
                        self._definition = None
                        self.id = None
                        self.e_num = None
                    def definition(self):
                        if self._definition is not None: return self._definition
                        _obj = InputObject()
                        _obj.create("id", MinOccurs=1, MaxOccurs=1, MinValInc=1, Action=storeInt)
                        _obj.add("e_num", self._e_num().definition(), MinOccurs=1)
                        self._definition = _obj
                        return self._definition
                    def createFrom(self, dr:DeserializedResult):
                        result = self.__class__()
                        if dr["id"] is not None:
                            result.id = dr["id"]
                        if len(dr["e_num"]) > 0:
                            result.e_num = [self._e_num().createFrom(it) for it in dr["e_num"]]
                        return result

                    class _e_num:
                        def __init__(self, params=None):
                            self._definition = None
                            self.id = None
                            self.value = None
                        def definition(self):
                            if self._definition is not None: return self._definition
                            _obj = InputObject()
                            _obj.create("id", MinOccurs=1, Enums=[*EnumList], Action=storeStr)
                            _obj.create("value", MinValInc=0, MaxValInc=1, Action=storeFloat)
                            self._definition = _obj
                            return self._definition
                        def createFrom(self, dr:DeserializedResult):
                            result = self.__class__()
                            if dr["id"] is not None:
                                result.id = dr["id"]
                            if len(dr["value"]) > 0:
                                result.value = [it.storedResult() for it in dr["value"]]
                            return result

                class _fffs:
                    def __init__(self, params=None):
                        self._definition = None
                        self.id = None
                        self.f_num = None
                    def definition(self):
                        if self._definition is not None: return self._definition
                        _obj = InputObject()
                        _obj.create("id", MinOccurs=1, MaxOccurs=1, MinValInc=1, Action=storeInt)
                        _obj.add("f_num", self._f_num().definition(), MinOccurs=1)
                        self._definition = _obj
                        return self._definition
                    def createFrom(self, dr:DeserializedResult):
                        result = self.__class__()
                        if dr["id"] is not None:
                            result.id = dr["id"]
                        if len(dr["f_num"]) > 0:
                            result.f_num = [self._f_num().createFrom(it) for it in dr["f_num"]]
                        return result

                    class _f_num:
                        def __init__(self, params=None):
                            self._definition = None
                            self.id = None
                            self.value = None
                        def definition(self):
                            if self._definition is not None: return self._definition
                            _obj = InputObject()
                            _obj.create("id", MinOccurs=1, MaxOccurs=1, Enums=[*EnumList], Action=storeStr)
                            _obj.create("value", MinOccurs=1, MaxOccurs=1, MinValInc=0, MaxValInc=1, Action=storeFloat)
                            self._definition = _obj
                            return self._definition
                        def createFrom(self, dr:DeserializedResult):
                            result = self.__class__()
                            if dr["id"] is not None:
                                result.id = dr["id"]
                            if dr["value"] is not None:
                                result.value = dr["value"]
                            return result

        class _bars:
            def __init__(self, params=None):
                self._definition = None
                self.typeone = None
                self.typetwo = None
                self.typethree = None
                self.typefour = None
            def definition(self):
                if self._definition is not None: return self._definition
                _obj = InputObject()
                _obj.add("typeone", self._typeone().definition())
                _obj.add("typetwo", self._typetwo().definition())
                _obj.add("typethree", self._typethree().definition())
                _obj.add("typefour", self._typefour().definition())
                _obj.addUniqueConstraint([ "typeone/value", \
                                           "typetwo/value", \
                                           "typethree/value", \
                                           "typefour/value" ])
                _obj.addAtLeastConstraint(SourcePredicatedTarget( \
                    target={ "typeone":None, \
                             "typetwo":None, \
                             "typethree":None, \
                             "typefour":None }))
                self._definition = _obj
                return self._definition
            def createFrom(self, dr:DeserializedResult):
                result = self.__class__()
                if len(dr["typeone"]) > 0:
                    result.typeone = [self._typeone().createFrom(it) for it in dr["typeone"]]
                if len(dr["typetwo"]) > 0:
                    result.typetwo = [self._typetwo().createFrom(it) for it in dr["typetwo"]]
                if len(dr["typethree"]) > 0:
                    result.typethree = [self._typethree().createFrom(it) for it in dr["typethree"]]
                if len(dr["typefour"]) > 0:
                    result.typefour = [self._typefour().createFrom(it) for it in dr["typefour"]]
                return result

            class _typeone:
                def __init__(self, params=None):
                    self._definition = None
                    self.id = None
                    self.aaa = None
                    self.bbb = None
                def definition(self):
                    if self._definition is not None: return self._definition
                    _obj = InputObject()
                    _obj.create("id", MinOccurs=1, MaxOccurs=1, Action=storeStr)
                    _obj.create("aaa", MinOccurs=1, MaxOccurs=1) \
                        .create("value", Enums=["x", "y", "z"], Action=storeStr)
                    _obj.create("bbb", MinOccurs=1, MaxOccurs=1) \
                        .create("value", MinValExc=0, Action=storeFloat)
                    self._definition = _obj
                    return self._definition
                def createFrom(self, dr:DeserializedResult):
                    result = self.__class__()
                    if dr["id"] is not None:
                        result.id = dr["id"]
                    if len(dr["aaa"]) > 0:
                        result.aaa = dr["aaa"].valuelist()
                    if len(dr["bbb"]) > 0:
                        result.bbb = dr["bbb"].valuelist()
                    return result

            class _typetwo:
                def __init__(self, params=None):
                    self._definition = None
                    self.id = None
                    self.x = None
                    self.y = None
                    self.z = None
                def definition(self):
                    if self._definition is not None: return self._definition
                    _obj = InputObject()
                    _obj.create("id", MinOccurs=1, MaxOccurs=1, Action=storeStr)
                    _obj.create("x", MaxOccurs=1) \
                        .create("value", Action=storeFloat)
                    _obj.create("y", MaxOccurs=1) \
                        .create("value", Action=storeFloat)
                    _obj.create("z", MaxOccurs=1) \
                        .create("value", Action=storeFloat)
                    _obj.addAtLeastConstraint(SourcePredicatedTarget( \
                        target={ "x":None, \
                                 "y":None, \
                                 "z":None }))
                    self._definition = _obj
                    return self._definition
                def createFrom(self, dr:DeserializedResult):
                    result = self.__class__()
                    if dr["id"] is not None:
                        result.id = dr["id"]
                    if len(dr["x"]) > 0:
                        result.x = dr["x"].valuelist()
                    if len(dr["y"]) > 0:
                        result.y = dr["y"].valuelist()
                    if len(dr["z"]) > 0:
                        result.z = dr["z"].valuelist()
                    return result

            class _typethree:
                def __init__(self, params=None):
                    self._definition = None
                    self.id = None
                    self.aaa = None
                    self.bbb = None
                    self.ccc = None
                def definition(self):
                    if self._definition is not None: return self._definition
                    _obj = InputObject()
                    _obj.create("id", MinOccurs=1, MaxOccurs=1, Action=storeStr)
                    _obj.create("aaa", MinOccurs=1, MaxOccurs=1) \
                        .create("value", Enums=["x", "y", "z"], Action=storeStr)
                    _obj.create("bbb", MinOccurs=0, MaxOccurs=1) \
                        .create("value", MinValExc=0, Action=storeFloat)
                    _obj.create("ccc", MinOccurs=0, MaxOccurs=1) \
                        .create("value", MinValExc=0, Action=storeFloat)
                    _obj.addExactlyConstraint(SourcePredicatedTarget( \
                        target={ "bbb":None, \
                                 "ccc":None }))
                    self._definition = _obj
                    return self._definition
                def createFrom(self, dr:DeserializedResult):
                    result = self.__class__()
                    if dr["id"] is not None:
                        result.id = dr["id"]
                    if len(dr["aaa"]) > 0:
                        result.aaa = dr["aaa"].valuelist()
                    if len(dr["bbb"]) > 0:
                        result.bbb = dr["bbb"].valuelist()
                    if len(dr["ccc"]) > 0:
                        result.ccc = dr["ccc"].valuelist()
                    return result

            class _typefour:
                def __init__(self, params=None):
                    self._definition = None
                    self.id = None
                    self.bbb = None
                def definition(self):
                    if self._definition is not None: return self._definition
                    _obj = InputObject()
                    _obj.create("id", MinOccurs=1, MaxOccurs=1, Action=storeStr)
                    _obj.create("bbb", MinOccurs=1, MaxOccurs=1) \
                        .create("value", MinValExc=0, Action=storeFloat)
                    self._definition = _obj
                    return self._definition
                def createFrom(self, dr:DeserializedResult):
                    result = self.__class__()
                    if dr["id"] is not None:
                        result.id = dr["id"]
                    if len(dr["bbb"]) > 0:
                        result.bbb = dr["bbb"].valuelist()
                    return result

    class _object_two:
        def __init__(self, params=None):
            self._definition = None
            self.circle = None
            self.square = None
        def definition(self):
            if self._definition is not None: return self._definition
            _obj = InputObject()
            _obj.add("circle", self._circle().definition(), MinOccurs=0, MaxOccurs=1)
            _obj.add("square", self._square().definition(), MinOccurs=0, MaxOccurs=1)
            self._definition = _obj
            return self._definition
        def createFrom(self, dr:DeserializedResult):
            result = self.__class__()
            if len(dr["circle"]) > 0:
                result.circle = self._circle().createFrom(dr["circle"])
            if len(dr["square"]) > 0:
                result.square = self._square().createFrom(dr["square"])
            return result

        class _circle:
            def __init__(self, params=None):
                self._definition = None
                self.name = None
                self.phone = None
            def definition(self):
                if self._definition is not None: return self._definition
                _obj = InputObject()
                _obj.create("name", MinOccurs=1, MaxOccurs=1) \
                    .create("value", Enums=["henry", "susie", "john", "martha"], Action=storeStr)
                _obj.create("phone", MinOccurs=1, MaxOccurs=1) \
                    .create("value", MinValInc=0, Action=storeInt)
                self._definition = _obj
                return self._definition
            def createFrom(self, dr:DeserializedResult):
                result = self.__class__()
                if len(dr["name"]) > 0:
                    result.name = dr["name"].valuelist()
                if len(dr["phone"]) > 0:
                    result.phone = dr["phone"].valuelist()
                return result

        class _square:
            def __init__(self, params=None):
                self._definition = None
                self.job = None
                self.sport = None
                self.triangle = None
                self.pentagon = None
            def definition(self):
                if self._definition is not None: return self._definition
                _obj = InputObject()
                _obj.create("job", MinOccurs=1, MaxOccurs=1) \
                    .create("value", Enums=["lawyer", "teacher", "doctor"], Action=storeStr)
                _obj.create("sport", MinOccurs=1, MaxOccurs=1) \
                    .create("value", Enums=["tennis", "football", "basketball"], Action=storeStr)
                _obj.add("triangle", self._triangle().definition(), MinOccurs=0, MaxOccurs=1)
                _obj.add("pentagon", self._pentagon().definition(), MinOccurs=0, MaxOccurs=1)
                self._definition = _obj
                return self._definition
            def createFrom(self, dr:DeserializedResult):
                result = self.__class__()
                if len(dr["job"]) > 0:
                    result.job = dr["job"].valuelist()
                if len(dr["sport"]) > 0:
                    result.sport = dr["sport"].valuelist()
                if len(dr["triangle"]) > 0:
                    result.triangle = self._triangle().createFrom(dr["triangle"])
                if len(dr["pentagon"]) > 0:
                    result.pentagon = self._pentagon().createFrom(dr["pentagon"])
                return result

            class _triangle:
                def __init__(self, params=None):
                    self._definition = None
                    self.sides = None
                    self.type = None
                def definition(self):
                    if self._definition is not None: return self._definition
                    _obj = InputObject()
                    _obj.create("sides", MinOccurs=1, MaxOccurs=1) \
                        .create("value", MinValInc=1, Action=storeInt)
                    _obj.create("type", MinOccurs=1, MaxOccurs=1) \
                        .create("value", Enums=["equilateral", "isosceles", "scalene"], Action=storeStr)
                    self._definition = _obj
                    return self._definition
                def createFrom(self, dr:DeserializedResult):
                    result = self.__class__()
                    if len(dr["sides"]) > 0:
                        result.sides = dr["sides"].valuelist()
                    if len(dr["type"]) > 0:
                        result.type = dr["type"].valuelist()
                    return result

            class _pentagon:
                def __init__(self, params=None):
                    self._definition = None
                    self.rise = None
                    self.run = None
                    self.foo_name_choices = None
                def definition(self):
                    if self._definition is not None: return self._definition
                    _obj = InputObject()
                    _obj.create("rise", MinOccurs=1, MaxOccurs=1) \
                        .create("value", MinValExc=0, Action=storeFloat)
                    _obj.create("run", MinOccurs=1, MaxOccurs=1) \
                        .create("value", MinValExc=0, Action=storeFloat)
                    _obj.create("foo_name_choices", MaxOccurs=1) \
                        .create("value", MinOccurs=0, Action=storeStr)
                    self._definition = _obj
                    return self._definition
                def createFrom(self, dr:DeserializedResult):
                    result = self.__class__()
                    if len(dr["rise"]) > 0:
                        result.rise = dr["rise"].valuelist()
                    if len(dr["run"]) > 0:
                        result.run = dr["run"].valuelist()
                    if len(dr["foo_name_choices"]) > 0:
                        result.foo_name_choices = dr["foo_name_choices"].valuelist()
                    return result

EnumList = ["ac206", "ac207", "ac208", "ac235", "ac236", "ac209", "ac210", "ac211",
            "ac212", "ac213", "ac214", "ac215", "ac216", "ac217", "ac218", "ac219",
            "ac220", "ac221", "ac222", "ac223", "ac224", "ac225", "ac226", "ac227",
            "ac228", "ac229", "ac230", "ac231", "ac232", "ac233"]
