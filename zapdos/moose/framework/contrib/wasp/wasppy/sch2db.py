import sys
from dataclasses import dataclass
from collections import defaultdict
from wasp import *

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

indent_space = '    '
indent_level = 0
schema_rules = { 'ChildAtLeastOne',    'ChildAtMostOne',     'ChildCountEqual',
                 'ChildExactlyOne',    'ChildUniqueness',    'DecreaseOver',
                 'ExistsIn',           'IncreaseOver',       'MaxOccurs',
                 'MaxValExc',          'MaxValInc',          'MinOccurs',
                 'MinValExc',          'MinValInc',          'NotExistsIn',
                 'SumOver',            'SumOverGroup',       'ValEnums',
                 'ValType',            'Description',        'InputChoices',
                 'InputDefault',       'InputName',          'InputTmpl',
                 'InputType',          'InputVariants' }

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def indent():
    return indent_space * indent_level

def is_dec_or_rule(node):
    return node.isDecorative() or node.name() in schema_rules

def is_existsin(node):
    return node.name() == 'ExistsIn'

def is_uniqueness(node):
    return node.name() == 'ChildUniqueness'

def is_atmostone(node):
    return node.name() == 'ChildAtMostOne'

def is_exactlyone(node):
    return node.name() == 'ChildExactlyOne'

def is_atleastone(node):
    return node.name() == 'ChildAtLeastOne'

def is_val(node):
    return node.name() == 'value'

def is_param_val(node):
    return is_val(node) and node.parent() and not is_object(node.parent())

def is_id(node):
    return node.name() == 'id' and len(node.value) == 0

def is_param(node):
    if is_dec_or_rule(node): return False
    for child in node:
        if not is_dec_or_rule(child) and not is_val(child):
            return False
    return True

def is_id_param(node):
    if is_dec_or_rule(node) or len(node.id) == 0 or len(node.id[0].value) > 0:
        return False
    for child in node:
        if not is_dec_or_rule(child) and not is_val(child) and not is_id(child):
            return False
    return True

def is_object(node):
    if is_dec_or_rule(node): return False
    for child in node:
        if not is_dec_or_rule(child) and not is_val(child) and not is_id(child):
            return True
    return False

def is_list(node):
    if is_dec_or_rule(node): return False
    for child in node:
        if not child.isDecorative() and not is_val(child):
            return False
    return len(node.value) > 0

def is_single(node):
    return len(node.MaxOccurs) > 0 and int(node.MaxOccurs[0]) == 1

def is_val_single(node):
    return len(node.value) > 0 and is_single(node.value[0])

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def get_desc_attr(node):
    desc_attr = ''
    if len(node.Description) > 0:
        desc_attr += ', Desc="' + str(node.Description[0]) + '"'
    return desc_attr

def is_number(value):
    if value is None: return False
    try: float(value)
    except ValueError: return False
    return True

def is_int(value):
    if value is None: return False
    try: int(value)
    except ValueError: return False
    return True

def get_occ_attrs(node):
    occ_attrs = ''
    for rule in ['MinOccurs', 'MaxOccurs']:
        if len(node[rule]) > 0:
            rule_val = str(node[rule][0])
            if is_int(rule_val):
                occ_attrs += ', ' + rule + '=' + rule_val
    return occ_attrs

def get_val_occ_attrs(node):
    if len(node.value) > 0: return get_occ_attrs(node.value[0])
    else:                   return ''

def get_range_attr(node, rule):
    rule_val = ''
    if len(node.value) > 0 and len(node.value[0][rule]) > 0:
        rule_val = str(node.value[0][rule][0])
    elif len(node[rule]) > 0:
        rule_val = str(node[rule][0])
    if is_number(rule_val): return ', ' + rule + '=' + rule_val
    else:                   return ''

def get_enums_attr(node):
    rule_val = None
    if len(node.value) > 0 and len(node.value[0].ValEnums) > 0:
        rule_val = node.value[0].ValEnums[0]
    elif len(node.ValEnums) > 0:
        rule_val = node.ValEnums[0]
    enums_attr = ''
    if rule_val:
        enums_attr = ', Enums=['
        for enum in rule_val:
            if not enum.isDecorative():
                if enum.name() == "REF": enums_attr += '*' + str(enum) + ', '
                else:                    enums_attr += '"' + str(enum) + '", '
        enums_attr = enums_attr[:-2] + ']'
    return enums_attr

def get_val_attrs(node):
    val_attrs = ''
    val_attrs += get_range_attr(node, 'MinValInc')
    val_attrs += get_range_attr(node, 'MinValExc')
    val_attrs += get_range_attr(node, 'MaxValInc')
    val_attrs += get_range_attr(node, 'MaxValExc')
    val_attrs += get_enums_attr(node)
    return val_attrs

def get_val_type(node):
    val_type = ''
    if len(node.value) > 0 and len(node.value[0].ValType) > 0:
        val_type = str(node.value[0].ValType[0])
    elif len(node.ValType) > 0:
        val_type = str(node.ValType[0])
    return val_type

def get_val_store(node):
    val_type = get_val_type(node)
    if   val_type == 'Int':  val_store = 'Int'
    elif val_type == 'Real': val_store = 'Float'
    else:                    val_store = 'Str'
    return ', Action=store' + val_store

def has_existsin(node):
    if (len(node.value) > 0 and len(node.value[0].ExistsIn) > 0 or
        len(node.ExistsIn) > 0): return True
    return False

def get_quote(val_type, str_val):
    if   val_type == 'Int'  and is_int(str_val):    quote = ''
    elif val_type == 'Real' and is_number(str_val): quote = ''
    else:                                           quote = '"'
    return quote

def get_def_attr(node):
    if has_existsin(node): return ''
    rule_val = ''
    if len(node.value) > 0 and len(node.value[0].InputDefault) > 0:
        rule_val = str(node.value[0].InputDefault[0])
    elif len(node.InputDefault) > 0:
        rule_val = str(node.InputDefault[0])
    rule_val = rule_val.strip(' "\'\n\t')
    def_attr = ''
    if len(rule_val) > 0:
        val_type = get_val_type(node)
        if (not is_val(node) and not is_val_single(node) or
                is_val(node) and not is_single(node)):
            val_list = ''
            for item in rule_val.split():
                quote = get_quote(val_type, item)
                val_list += quote + item + quote + ', '
            def_val = '[' + val_list[:-2] + ']'
        else:
            quote = get_quote(val_type, rule_val)
            def_val = quote + rule_val + quote
        def_attr = ', Default=' + def_val
    return def_attr

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def write_exists_constraint(exists_info):
    global indent_level
    indent_level += 1
    print(indent() + '[ "' + exists_info.source + '" ], \\')
    paths = ''
    for target_path in exists_info.targets:
        paths += indent() + str(' ' * 9) + '"' + target_path + '", \\\n'
    paths = paths[len(indent()) + 9 : -4]
    print(indent() + 'target=[ ' + paths + ' ]', end='')
    if not any([exists_info.extras, exists_info.refs]): print('))')
    else:
        print(', \\')
        discrete_values = ''
        for extra_value in exists_info.extras:
            discrete_values += '"' + extra_value + '", '
        for ref_value in exists_info.refs:
            discrete_values += '*' + ref_value + ', '
        print(indent() + 'discrete=[ ' + discrete_values[:-2] + ' ]))')
    indent_level -= 1

def write_unique_constraint(unique_info):
    paths = ''
    for target_path in unique_info.targets:
        paths += indent() + str(' ' * 27) + '"' + target_path + '", \\\n'
    paths = paths[len(indent()) + 27 : -4]
    print('[ ' + paths + ' ])')

def write_select_constraint(select_info):
    global indent_level
    indent_level += 1
    if len(select_info.source) > 0:
        print(indent() + 'source="' + select_info.source + '", \\')
    items = ''
    for target_path, target_vals in select_info.targets.items():
        rule_path = '"' + target_path + '"'
        if len(target_vals) == 1:
            item = target_vals[0]
            rule_val = 'None' if item is None else '"' + item + '"'
        else:
            val_list = ''
            for item in target_vals:
                val_item = 'None' if item is None else '"' + item + '"'
                val_list += val_item + ', '
            rule_val = '[' + val_list[:-2] + ']'
        items += indent() + str(' ' * 9) + rule_path + ':' + rule_val + ', \\\n'
    items = items[len(indent()) + 9 : -4]
    print(indent() + 'target={ ' + items + ' }))')
    indent_level -= 1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def write_initialize_method(node):
    global indent_level
    print(indent() + 'def __init__(self, params=None):')
    indent_level += 1
    print(indent() + 'self._definition = None')
    for child in node:
        if not is_dec_or_rule(child):
            print(indent() + 'self.' + child.name() + ' = None')
    indent_level -= 1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def write_createfrom_method(node):
    global indent_level
    print(indent() + 'def createFrom(self, dr:DeserializedResult):')
    indent_level += 1
    print(indent() + 'result = self.__class__()')

    for child in node:

        if is_id(child) or is_param_val(child):
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            # if dr["id"] is not None: result._id = dr["id"]
            # or
            # if dr["value"] is not None: result._value = dr["value"]
            # or
            # if len(dr["value"]) > 0: result.value = [it.storedResult() for it in dr["value"]]
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if is_param_val(child) and not is_single(child):
                print(indent() + 'if len(dr["value"]) > 0:')
                indent_level += 1
                print(indent() + 'result.value = [it.storedResult() for it in dr["value"]]')
                indent_level -= 1
            else:
                print(indent() + 'if dr["' + child.name() + '"] is not None:')
                indent_level += 1
                print(indent() + 'result.' + child.name() + ' = dr["' + child.name() + '"]')
                indent_level -= 1

        elif is_param(child):
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            # if len(dr["param_name"]) > 0:
            #     result._param_name = dr["param_name"].value()
            #     or
            #     result._param_name = dr["param_name"].valuelist()
            #     or
            #     result._param_name = [it.value() for it in dr["param_name"]]
            #     or
            #     result._param_name = [it.valuelist() for it in dr["param_name"]]
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            print(indent() + 'if len(dr["' + child.name() + '"]) > 0:')
            indent_level += 1
            value_access = 'value' if is_val_single(child) else 'valuelist'
            print(indent() + 'result.' + child.name() + ' = ', end='')
            if is_single(child):
                print('dr["' + child.name() + '"].' + value_access + '()')
            else:
                print('[it.' + value_access + '() for it in dr["' + child.name() + '"]]')
            indent_level -= 1

        elif is_object(child) or is_id_param(child):
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            # if len(dr["subobj"]) > 0:
            #     result._subobj = self.subobj().createFrom(dr["subobj"])
            #     or
            #     result._subobj = [self.subobj().createFrom(it) for it in dr["subobj"]]
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            print(indent() + 'if len(dr["' + child.name() + '"]) > 0:')
            indent_level += 1
            if is_single(child):
                print(indent() + 'result.' + child.name() + ' = self._' + child.name()
                      + '().createFrom(dr["' + child.name() + '"])')
            else:
                print(indent() + 'result.' + child.name() + ' = [self._' + child.name()
                      + '().createFrom(it) for it in dr["' + child.name() + '"]]')
            indent_level -= 1

    print(indent() + 'return result')
    indent_level -= 1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def write_definition_method(node, target_rules):
    global indent_level
    print(indent() + 'def definition(self):')
    indent_level += 1
    print(indent() + 'if self._definition is not None: return self._definition')
    print(indent() + '_obj = InputObject(' + get_desc_attr(node)[2:] + ')')

    for child in node:

        if is_id(child) or is_param_val(child):
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            # _obj.create("id", (Min,Max)Occurs=X, (Min,Max)Val(Exc,Inc)=Y, Action=store(Int,Float,Str)) or
            # _obj.create("value", (Min,Max)Occurs=X, (Min,Max)Val(Exc,Inc)=Y, Action=store(Int,Float,Str))
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            print(indent() + '_obj.create("' + child.name()
                  + '"' + get_occ_attrs(child) + get_val_attrs(child)
                  + get_def_attr(child) + get_val_store(child) + ')')

        elif is_param(child):
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            # _obj.create("param_name", (Min,Max)Occurs=X, Default=Z, Desc="Description") \
            #     .create("value", (Min,Max)Occurs=X, (Min,Max)Val(Exc,Inc)=Y, Action=store(Int,Float,Str))
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            print(indent() + '_obj.create("' + child.name()
                  + '"' + get_occ_attrs(child) + get_def_attr(child)
                  + get_desc_attr(child) + ') \\')
            indent_level += 1
            print(indent() + '.create("value"'
                  + get_val_occ_attrs(child) + get_val_attrs(child)
                  + get_val_store(child) + ')')
            indent_level -= 1

        elif is_object(child) or is_id_param(child):
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            # _obj.add("subobj", self._subobj().definition(), (Min,Max)Occurs=X)
            # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            print(indent() + '_obj.add("' + child.name()
                  + '", self._' + child.name() + '().definition()'
                  + get_occ_attrs(child) + ')')

    if exists_info_list := target_rules['exists'].get(node._nodeview.path()):
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        # _obj.addExistsConstraint(ExistsConstraint( \
        #     [ "path/to/terminal/source/context" ], \
        #     target=[ "path/to/terminal/target/01", \
        #              "path/to/terminal/target/02", \
        #              "path/to/terminal/target/03", \
        #              "path/to/terminal/target/04" ], \
        #     discrete=[ "extra_01", "extra_02", *ref_01, *ref_02 ]))
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        for exists_info in exists_info_list:
            print(indent() + '_obj.addExistsConstraint(ExistsConstraint( \\')
            write_exists_constraint(exists_info)

    if unique_info_list := target_rules['uniques'].get(node._nodeview.path()):
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        # _obj.addUniqueConstraint([ "path/to/terminal/target/01", \
        #                            "path/to/terminal/target/02", \
        #                            "path/to/terminal/target/03", \
        #                            "path/to/terminal/target/04" ])
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        for unique_info in unique_info_list:
            print(indent() + '_obj.addUniqueConstraint(', end='')
            write_unique_constraint(unique_info)

    if atmost_info_list := target_rules['atmost'].get(node._nodeview.path()):
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        # _obj.addAtMostConstraint(SourcePredicatedTarget( \
        #     source="path/to/select/root/source/context", \
        #     target={ "path/to/some/target/context":None, \
        #              "path/to/terminal/target":"value" }))
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        for atmost_info in atmost_info_list:
            print(indent() + '_obj.addAtMostConstraint(SourcePredicatedTarget( \\')
            write_select_constraint(atmost_info)

    if exactly_info_list := target_rules['exactly'].get(node._nodeview.path()):
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        # _obj.addExactlyConstraint(SourcePredicatedTarget( \
        #     source="path/to/select/root/source/context", \
        #     target={ "path/to/some/target/context":None, \
        #              "path/to/terminal/target":"value" }))
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        for exactly_info in exactly_info_list:
            print(indent() + '_obj.addExactlyConstraint(SourcePredicatedTarget( \\')
            write_select_constraint(exactly_info)

    if atleast_info_list := target_rules['atleast'].get(node._nodeview.path()):
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        # _obj.addAtLeastConstraint(SourcePredicatedTarget( \
        #     source="path/to/select/root/source/context", \
        #     target={ "path/to/some/target/context":None, \
        #              "path/to/terminal/target":"value" }))
        # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        for atleast_info in atleast_info_list:
            print(indent() + '_obj.addAtLeastConstraint(SourcePredicatedTarget( \\')
            write_select_constraint(atleast_info)

    print(indent() + 'self._definition = _obj')
    print(indent() + 'return self._definition')
    indent_level -= 1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def write_class(node, target_rules):
    global indent_level
    print('\n' + indent() + 'class _' + node.name() + ':')
    indent_level += 1
    write_initialize_method(node)
    write_definition_method(node, target_rules)
    write_createfrom_method(node)
    for child in node:
        if is_object(child) or is_id_param(child):
            write_class(child, target_rules)
    indent_level -= 1

def write_enums(node):
    cols = 8
    decl = node.name() + ' = ['
    vals = node.child_by_name('value')
    for i in range(0, len(vals), cols):
        row_values = vals[i:i + cols]
        row_output = ', '.join(f'"{item}"' for item in row_values)
        row_prefix = '\n' + decl if i == 0 else str(' ' * len(decl))
        row_suffix = ',' if i + cols < len(vals) else ']'
        print(row_prefix + row_output + row_suffix)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

@dataclass
class exists_info:
    source: str
    targets: list
    extras: list
    refs: list

@dataclass
class unique_info:
    targets: list

@dataclass
class select_info:
    source: str
    targets: dict

def first_no_backtrack_index(path):
    for index, char in enumerate(path):
        if char not in ['.', '/']:
            return index
    return len(path)

def nth_slash_from_end_index(path, n):
    if n == 0: return len(path)
    count = 0
    for index, char in enumerate(reversed(path)):
        if char == '/':
            count += 1
            if count == n: return len(path) - 1 - index
    return 0

def nth_slash_from_beg_index(path, n):
    if n == 0: return 0
    count = 0
    for index, char in enumerate(path):
        if char == '/':
            count += 1
            if count == n: return index
    return len(path)

def need_val_level(path):
    return not (path in ['id', 'value'] or path.endswith(('/id', '/value')))

def get_relative_path(sch_context, sch_target):
    backtrack_count = sch_context.count('/') - 1
    one_level_index = nth_slash_from_beg_index(sch_target, 2)
    sch_target_beg = str('../' * backtrack_count)
    sch_target_end = sch_target[one_level_index + 1:]
    sch_target_rel = sch_target_beg + sch_target_end
    return sch_target_rel

def get_most_backtracks(sch_targets):
    most_backtracks = 0
    for sch_target in sch_targets:
        non_backtrack_i = first_no_backtrack_index(sch_target)
        backtracks_path = sch_target[:non_backtrack_i]
        this_backtracks = backtracks_path.count('../')
        if this_backtracks > most_backtracks:
            most_backtracks = this_backtracks
    return most_backtracks

def get_db_target(most_backtracks, sch_target, db_source):
    non_backtrack_i = first_no_backtrack_index(sch_target)
    backtracks_path = sch_target[:non_backtrack_i]
    this_backtracks = backtracks_path.count('../')
    levels_to_reuse = most_backtracks - this_backtracks
    slash_beg_index = nth_slash_from_beg_index(db_source, levels_to_reuse)
    db_target_beg = db_source[:slash_beg_index]
    db_target_mid = '/' if db_target_beg else ''
    db_target_end = sch_target[non_backtrack_i:]
    db_target = db_target_beg + db_target_mid + db_target_end
    return db_target

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def convert_exists_and_store(sch_context, sch_targets, sch_extras, sch_refs, exists):

    # walk over target list and change any absolute paths into relative paths
    for i in range(len(sch_targets)):
        sch_target = sch_targets[i]
        if sch_target.startswith('/'):
            sch_target_rel = get_relative_path(sch_context, sch_target)
            sch_targets[i] = sch_target_rel

    # use top level of targets to determine database context and source paths
    most_backtracks = get_most_backtracks(sch_targets)
    slash_end_index = nth_slash_from_end_index(sch_context, most_backtracks)
    db_context = sch_context[:slash_end_index]
    db_source  = sch_context[slash_end_index+1:]

    # all source paths must be to terminal nodes so add value level if needed
    if need_val_level(db_source): db_source += '/value'

    db_targets = []
    for sch_target in sch_targets:

        # build database target path out of schema target and database source
        db_target = get_db_target(most_backtracks, sch_target, db_source)

        # target paths must be to terminal nodes so add value level if needed
        if need_val_level(db_target): db_target += '/value'

        db_targets.append(db_target)

    exists[db_context].append(exists_info(db_source, db_targets, sch_extras, sch_refs))

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def convert_unique_and_store(sch_context, sch_targets, node, uniques):

    db_targets = []
    for sch_target in sch_targets:

        # unique paths should not reference parent context so error if needed
        assert not sch_target.startswith('../'), (
            'ChildUniqueness path "' + sch_target + '" references parent context')

        # context must map to class so fix paths if class will not be created
        if is_val(node) and node.parent() and is_param(node.parent()):
            parent_end_index = nth_slash_from_end_index(sch_context, 2)
            db_context = sch_context[:parent_end_index]
            db_target  = sch_context[parent_end_index+1:] + '/value'
        elif is_param(node):
            parent_end_index = nth_slash_from_end_index(sch_context, 1)
            db_context = sch_context[:parent_end_index]
            db_target  = sch_context[parent_end_index+1:] + '/value'
        else:
            db_context = sch_context
            db_target  = sch_target

        # unique paths must be to terminal nodes so add value level if needed
        if need_val_level(db_target): db_target += '/value'

        db_targets.append(db_target)

    uniques[db_context].append(unique_info(db_targets))

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def convert_select_and_store(sch_context, sch_targets, selects):

    # walk over target list and change any absolute paths into relative paths
    for sch_target in list(sch_targets.keys()):
        if sch_target.startswith('/'):
            sch_target_rel = get_relative_path(sch_context, sch_target)
            sch_targets[sch_target_rel] = sch_targets.pop(sch_target)

    # use top level of targets to determine database context and source paths
    most_backtracks = get_most_backtracks(sch_targets)
    slash_end_index = nth_slash_from_end_index(sch_context, most_backtracks)
    db_context = sch_context[:slash_end_index]
    db_source  = sch_context[slash_end_index+1:]

    db_targets = {}
    for sch_target in sch_targets:

        # build database target path out of schema target and database source
        db_target = get_db_target(most_backtracks, sch_target, db_source)

        # assign paths must be to terminal nodes so add value level if needed
        if any(item is not None for item in sch_targets[sch_target]):
            if need_val_level(db_target): db_target += '/value'

        db_targets[db_target] = sch_targets[sch_target]

    selects[db_context].append(select_info(db_source, db_targets))

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def gather_target_rules(node, target_rules = defaultdict(lambda: defaultdict(list))):

    for child in node:

        if is_existsin(child):
            sch_context = node._nodeview.path()
            sch_targets = [str(val) for val in child.child_by_name('value')]
            sch_extras  = [str(val) for val in child.child_by_name('EXTRA')]
            sch_refs    = [str(val) for val in child.child_by_name('EXTRAREF')]
            exists = target_rules['exists']
            convert_exists_and_store(sch_context, sch_targets, sch_extras, sch_refs, exists)

        elif is_uniqueness(child):
            sch_context = node._nodeview.path()
            sch_targets = [str(val) for val in child.child_by_name('value')]
            uniques = target_rules['uniques']
            convert_unique_and_store(sch_context, sch_targets, node, uniques)

        elif is_atmostone(child) or is_exactlyone(child) or is_atleastone(child):
            sch_context = node._nodeview.path()
            sch_targets = defaultdict(list)
            for rule_child in child:
                if rule_child.isDecorative(): continue
                if len(rule_child) == 0:
                    rule_path = str(rule_child)
                    rule_val  = None
                else:
                    rule_path = rule_child.name()
                    rule_val  = str(rule_child)
                sch_targets[rule_path].append(rule_val)
            if is_atmostone(child):    selects = target_rules['atmost']
            elif is_exactlyone(child): selects = target_rules['exactly']
            elif is_atleastone(child): selects = target_rules['atleast']
            convert_select_and_store(sch_context, sch_targets, selects)

        if not is_dec_or_rule(child): gather_target_rules(child, target_rules)

    return target_rules

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

def write_database(schema_root):
    print('from wasp import *')
    print('from Database import *')
    target_rules = gather_target_rules(schema_root)
    for child in schema_root:
        if is_object(child): write_class(child, target_rules)
        elif is_list(child): write_enums(child)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: ' + __file__ + ' /path/to/schema')
        sys.exit(1)
    if not (schema := Interpreter(Syntax.SON, path=sys.argv[1])):
        print('Parse errors:', *schema.parseDiagnostics(), sep='\n')
        sys.exit(1)
    write_database(schema.root())
