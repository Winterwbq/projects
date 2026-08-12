# Wasp Python Interfaces (WaspPy)

WaspPy provides a python interface for utilizing WASP's parsing and validation functionality. The primary design consideration is to enable access to data represented in SON, DDI, EDDI, HIT, and HALITE formats.

WaspPy places an emphasis on client convenience and provides overloading of the ".", dot operator. This is intended to allow a client to interact with their input with calls of the following form, `doc.subnode1.subnode2`, where doc is the object containing the parsed input and `subnode*` are branches of the parse tree defined in either a static input schema file or inline via an InputObject definition database.


## Example

WASP supports structured and definition-driven syntaxes. Structured syntaxes (SON, HIT), do not require an input schema to be brought into memory. Definition-driven syntaxes (DDI, EDDI) require an input schema to construct the hierarchy of the desired parse tree. The following example illustrates how to bring input data into a program using structured and definition-driven syntax.

### Problem Description
The following fictional application is a general chemistry code describing salts and property interpolation. Specifically, the input is composed of a collection of salts and series of temperatures at which to query salt density.

The input schema that describes the input hierarchy and parameter constraint is [below](input_schema). A legal input follows and depicts 2 salts, `LiF` and `NaF`, and temperatures queries at `1100, 1200, 1300, 1400`:

```javascript
salts {
    % Favorite salt
    salt(LiF) {
        MeltTemp : 1121.2
        MolecularWeight : 25.9394
        BoilTemp : 2512
        Density
        {
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
            A : 2.76
            B : 6.36e-4
            MinTemp : 1273
            MaxTemp : 1373
        }
    }
}
queries {
    temperatures = [1100 1200 1300 1400]
}
```

#### Python program

```python
from wasp import *
import math

class LinearModel:
    ''' _b*x + _c*y = _a '''
    def __init__(self, params):
        self._a = 0.0
        self._b = 1.0
        self._c = 1.0
        self._minT = math.inf
        self._maxT = -math.inf

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

    def get_y(self, x: float) -> "float":
        if self._c == 0.0:
            return math.inf

        return (self._a - (self._b * x)) / self._c

class Salt:
    def __init__(self,params):
        self._name = ""
        self._molew = 0.0
        self._meltT = 0.0
        self._boilT = 0.0
        self._density: LinearModel

        # Loop over salt parameters
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

if __name__ == '__main__':
    import sys
    schemapath = "path/to/application/schema.sch or schema data"
    input_file = sys.argv[1]
    interpreter = Interpreter(Syntax.SON, schema=schemapath, path=input_file)

    errors = interpreter.errors()
    if errors:
        print ("\n".join(errors))
        sys.exit(1)

    document = interpreter.root()

    # Obtain required queries parameter
    queries = document.queries

    # Obtain required salts
    salts = []
    for component in document.salts.salt:
        salts.append(Salt(component))

    # Obtain each salt's melt temperature value
    for v in document.salts.salt.MeltTemp.value:
        # Print salt's id (located at ../../id of value node) and melt temperature
        print ("MeltTemp of", str(v.parent().parent().id), "is", float(v))

    # Obtain query temperatures
    temperatures = []
    for t in queries.temperatures.value:
        temperatures.append(float(t))

    # Evaluate salt density for each temperature
    for s in salts:
        for t in temperatures:
            print ("Salt", s._name, "density at",t, "is", s.density(t))

```

When executing the above program and providing the given input you can expect the following output:

```
MeltTemp of LiF is 1121.2
MeltTemp of NaF is 1268.0
Salt LiF density at 1100.0 is 1.82
Salt LiF density at 1200.0 is 1.77
Salt LiF density at 1300.0 is 1.7200000000000002
Salt LiF density at 1400.0 is 1.67
Salt NaF density at 1100.0 is 2.0603999999999996
Salt NaF density at 1200.0 is 1.9968
Salt NaF density at 1300.0 is 1.9331999999999998
Salt NaF density at 1400.0 is 1.8695999999999997
```

If an input error is encountered, as defined in the input schema, the program will emit a user-friendly diagnostic and exit. For example, if a `MaxTemp` value violates the `MinTemp` value constraint the following diagnostic is emitted.

```
line:12 column:29 - Validation Error: MaxTemp value "1367.5" is less than or equal to the allowed minimum exclusive value of "1523.6" from "../../MinTemp/value"
```


### Accessors

The dot operator provides the ability to navigate the hierarchy of the parse tree given the name of the subcomponents. When a subcomponent name conflicts with a Python reserved keyword the bracket operator `[]` can be used.

### Syntaxes
The syntax can be specified using the `Syntax.X` where `X` is one of `HIT`, `SON`, `DDI`, and `EDDI`.

For example, the input above is equivalent to the followiing HIT-formatted input and will produce the same out with only changing the one line:

```diff
- interpreter = Interpreter(Syntax.SON, schema=schemapath, path=input_file)
+ interpreter = Interpreter(Syntax.HIT, schema=schemapath, path=input_file)
...
```

```
[salts]
    # Favorite salt
    [salt]
        id = LiF
        MeltTemp = 1121.2
        MolecularWeight = 25.9394
        BoilTemp = 2512
        [Density]
            A = 2.37
            B = 5.0e-4
            MinTemp = 1123.6
            MaxTemp = 1367.5
        []
    []
    [salt]
        id = NaF
        MolecularWeight = 41.9882
        MeltTemp = 1268
        BoilTemp = 1978
        [Density]
            A = 2.76
            B = 6.36e-4
            MinTemp = 1273
            MaxTemp = 1373
        []
    []
[]
[queries]
    temperatures = '1100 1200 1300 1400'
[]
```

#### Input Schema
```javascript
salts{
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
                MinOccurs = 0
                MaxOccurs = 1
                value{
                    MinOccurs = 1
                    MaxOccurs = 1
                    ValType   = Real
                } % end value
            } % end A

            B{
                MinOccurs = 0
                MaxOccurs = 1
                value{
                    MinOccurs = 1
                    MaxOccurs = 1
                    ValType   = Real
                    MinValInc = 0
                } % end value
            } % end B

            MaxTemp{
                MinOccurs = 0
                MaxOccurs = 1
                value{
                    MinOccurs = 1
                    MaxOccurs = 1
                    ValType   = Real
                    MinValExc = "../../MinTemp/value"
                } % end value
            } % end MaxTemp

            MinTemp{
                MinOccurs = 0
                MaxOccurs = 1
                value{
                    MinOccurs = 1
                    MaxOccurs = 1
                    ValType   = Real
                    MinValExc = 0
                } % end value
            } % end MinTemp
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
```

#### Equivalent Program Using InputObject Definition
The Database.py module's InputObject allows Python programs to provide a definition of their input data with enhanced program-specific diagnostic abilities. Here is the same program updated with an embedded InputObject definition:

```python
from wasp import *
from Database import InputObject, storeFloat, storeStr
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
        result._maxT = do["MaxTemp"].value()

        # Conduct temperature check
        if result._maxT < result._minT:
            do.interpreter.createErrorDiagnostic(do["MaxTemp"].node,
            "value of "+str(result._maxT)+" is less than or equal to the allowed minimum exclusive value of "
            +str(result._minT)+ " located at "+do["MinTemp"].node.info()+"!")

        theType = do["Type"].value()

        # Require Type to be a supported enumeration
        # This demonstrates post deserialization diagnostic generation
        enumerations = result.definition()["Type"]["value"].enumerations()
        if theType not in enumerations:
            do.interpreter.createWarningDiagnostic(do["Type"].node, "has value of "+str(do["Type"].node)+" which is not listed in "+str(enumerations))

        return result

    ''' _b*x + _c*y = _a '''
    def __init__(self):
        self._a = 0.0
        self._b = 1.0
        self._c = 1.0
        self._minT = math.inf
        self._maxT = -math.inf

    def get_y(self,x: float) -> "float":
        if self._c == 0.0:
            return math.inf

        return (self._a - (self._b * x)) / self._c

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
                .createRequiredSingle("value", MinValExc=0, Action=storeFloat)
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
        result.id = do["id"] # not an id=value, just salt(id)
        result.moleweight = do["MolecularWeight"].value() # is a key=value MolecularWeight=value
        result.meltTemp = do["MeltTemp"].value()
        result.boilTemp = do["BoilTemp"].value()
        result._density = LinearModel.createFrom(do["Density"])

        return result

    def __init__(self):
        self.id = ""
        self.moleweight = 0.0
        self.meltTemp = 0.0
        self.boilTemp = 0.0
        self._density: LinearModel

    def density(self,T: float) -> "float":
        return self._density.get_y(T)

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
                .createRequired("value", MinValExc=0, Action=storeFloat)
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

if __name__ == '__main__':
    import sys
    input_file = sys.argv[1]
    interpreter = Interpreter(Syntax.SON, path=input_file)

    errors = interpreter.errors()
    if errors:
        print ("\n".join(errors))
        sys.exit(1)

    document = interpreter.root()

    # Obtain the input's definition database
    definition = TheInput.definition()

    # instance the user database with the interpreter user data
    db = definition.deserialize(interpreter.root(), interpreter)

    # Emit deserialize diagnostics and quit
    if interpreter.deserializeDiagnostics():
        print("".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        sys.exit(1)

    # instance the program input structure from the definition instanced user database
    theInput = TheInput.createFrom(db)

    # Emit creation diagnostics and quit
    if interpreter.deserializeDiagnostics():
        print("".join(str(x)+"\n" for x in interpreter.deserializeDiagnostics()))
        sys.exit(1)

    # Obtain each salt's melt temperature value
    for salt in theInput.salts:
        # Print salt's id and melt temperature
        print ("MeltTemp of", str(salt.id), "is", float(salt.meltTemp))

    # Evaluate salt density for each temperature
    for s in theInput.salts:
        for t in theInput.queryTemps:
            print ("Salt", s.id, "density at",t, "is", s.density(t))
```

When executed with the SON-formatted Salt input the expected output is produced:

```shell
MeltTemp of LiF is 1121.2
...
Salt NaF density at 1400.0 is 1.8695999999999997
```

Similarly, if a validation error is introduced an informative diagnostic is emitted with applicable providence of the issue:

```shell
problem.son:12.19: MaxTemp value of 1367.5 is less than or equal to the allowed minimum exclusive value of 1523.6 located at MinTemp on line 11 column 19!
 ```



