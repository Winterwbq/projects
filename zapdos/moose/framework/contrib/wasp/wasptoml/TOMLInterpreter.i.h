#ifndef WASP_TOMLINTERPRETER_I_H
#define WASP_TOMLINTERPRETER_I_H

#include "waspcore/wasp_bug.h"

template<class S>
TOMLInterpreter<S>::TOMLInterpreter(std::ostream& err) : Interpreter<S>(err)
    , m_parent(nullptr)
    , m_object(nullptr)
    , mHasFile(false)
{
}
template<class S>
TOMLInterpreter<S>::~TOMLInterpreter()
{
    if (m_object) delete m_object;
}

template<class S>
TOMLInterpreter<S>* TOMLInterpreter<S>::create_nested_interpreter(Interpreter<S>* parent)
{
    wasp_require(parent);
    auto* interp = new TOMLInterpreter<S>(parent->error_stream());
    wasp_insist(dynamic_cast<TOMLInterpreter<S>*>(parent) != nullptr,
        "parent interpreter must be the same type");
    interp->m_parent = dynamic_cast<TOMLInterpreter<S>*>(parent);

    // Make sure the nested interpreter knows to search relative from
    // the directory from which it is being included
    // NOTE: TOML doesn't support nested files
    interp->search_paths().push_back(wasp::dir_name(parent->stream_name()));
    return interp;
}

template<class S>
bool TOMLInterpreter<S>::parse(std::istream& in,
                                 std::size_t   startLine,
                                 std::size_t   startColumn)
{
    std::string name = Interpreter<S>::stream_name();
    if (name.empty()) name = "stream input";
    return Interpreter<S>::template parse_impl<TOMLParser>(
        in, name, startLine, startColumn);
}
template<class S>
bool TOMLInterpreter<S>::parseFile(const std::string& filename, size_t line)
{
    std::ifstream in(filename.c_str());
    if (!in.good())
    {
        Interpreter<S>::error_diagnostic()
            << position(&filename)
            << " is either inaccessible or doesn't exist! Unable to read."
            << std::endl;
        return false;
    }
    return parseStream(in, filename, line);
}
template<class S>
bool TOMLInterpreter<S>::assign_value(Value& v, const NodeView& n)
{
    wasp_require(!n.is_null());
    wasp_require(n.is_leaf()); 
    bool ok = true;

    switch (n.token_type())
    {
        case wasp::INTEGER:
            v = n.to_int(&ok);
            break;
        case wasp::REAL:
            v = n.to_double(&ok);
            break;
        case wasp::WASP_TRUE:
            v = true;
            break;
        case wasp::WASP_FALSE:
            v = false;
            break;
        default:
            v = n.to_string(&ok);
            break;
    }
    if (!ok) 
    {
        Super::error_diagnostic() << position(&Super::stream_name(), n.line(), n.column())
            << ": " << n.name() << " (" << n.data() << ") failed to convert!" << std::endl;
    }
    return ok;
}

template<class S>
bool TOMLInterpreter<S>::build_array(DataArray& array, const NodeView& parent)
{
    bool success = true;
    for (auto node : parent)
    {
        if (node.is_decorative()) continue;
        auto type = node.type();
        switch(type){            
            case wasp::OBJECT: // object/table ({...})
                array.push_back(DataObject());
                success &= build_object(array.back().as_object(), node);    
                break;
            case wasp::ARRAY: // array ([...])
                array.push_back(DataArray());
                success &= build_array(array.back().as_array(), node);
                break;
            case wasp::VALUE: // real, string, boolean
                array.push_back(Value());
                success &= assign_value(array.back(), node);
                break;
        }
    }
    return success;
}
template<class S>
bool TOMLInterpreter<S>::build_object(DataObject& object, const NodeView& parent)
{
   
    bool success = true;
    for (auto node : parent)
    {
        if (node.is_decorative()) continue;
        auto type = node.type();
        auto name = node.name();
        switch(type){
            case wasp::KEYED_VALUE: // key = value where value could be primitive, array, or object
                if (object.contains(name))
                {
                    Super::error_diagnostic() << position(&Super::stream_name(), node.line(), node.column())
                           << ": " << name << " is already defined!" << std::endl;
                    success &= false; continue;
                }
                {
                // determine if value is primitive, object, or array
                // RBRACE = OBJECT
                // RBRACKET = ARRAY
                // else VALUE/primitive
                const auto& last_node = node.child_at(node.child_count()-1);
                auto last_type = last_node.type();
                if (last_type == wasp::RBRACE)
                {
                    object[name] = DataObject();
                    // objects children are flat under the keyed-value, pass node
                    success &= build_object(object[name].as_object(), node);
                }
                else if (last_type == wasp::RBRACKET)
                {
                    object[name] = DataArray();
                    // array children are flat under the keyed-value, pass node
                    success &= build_array(object[name].as_array(), node);
                }
                else{
                    // pass value node
                    success &= assign_value(object[name], last_node);
                }
                }
                break;
            case wasp::TABLE: // table ([x])
                if (object.contains(name) && !object[name].is_object())
                {
                    Super::error_diagnostic() << position(&Super::stream_name(), node.line(), node.column())
                           << ": " << name << " is already defined but not as a table!" << std::endl;
                    success &= false; continue;
                }
                else{
                    object[name] = DataObject();                    
                }
                success &= build_object(object[name].as_object(), node);
                break;
            case wasp::ARRAY_TABLE: // array of tables ([[x]])
                if (object.contains(name) && !object[name].is_array())
                {
                    Super::error_diagnostic() << position(&Super::stream_name(), node.line(), node.column())
                           << ": " << name << " is already defined but not as an array!" << std::endl;
                    success &= false; continue;
                }
                // first encounter, create the array
                else if (!object.contains(name)){
                    // Instance an object in the named array
                    object[name] = DataArray();
                }
                object[name].as_array().push_back(DataObject());
                wasp_check(object.contains(name) && object[name].is_array() && !object[name].as_array().empty() && object[name].as_array().back().is_object());
                success &= build_object(object[name].as_array().back().as_object(), node);
                break;
            case wasp::IDENTIFIER: // x.y.z = abc or [x.y.z], or [[x.y.z]] (z is inline table, object, or array, but x,y need to be determined)
                if (object.contains(name))
                {
                    if (object[name].is_array())
                    {
                        wasp_check(!object[name].as_array().empty()); 
                        wasp_check(object[name].as_array().back().is_object());
                        // if already defined as an array, obtain the last array entry
                        // and continue building it given the new node
                        success &= build_object(object[name].as_array().back().as_object(), node);
                    }
                    else if (object[name].is_object())
                    {
                        // recurse into the identifier hierarchy
                        success &= build_object(object[name].as_object(), node);    
                    }
                    else // attempting to redefine obj[name]
                    {
                        Super::error_diagnostic() << position(&Super::stream_name(), node.line(), node.column())
                           << ": " << name << " is already defined!" << std::endl;
                        success &= false; continue;
                    }
                    
                }
                else // if we have not seen this component, create an object, and recurse into the identifier hierarchy
                {
                    object[name] = DataObject();
                    success &= build_object(object[name].as_object(), node);
                }

        }
    }
    return success;
}

template<class S>
bool TOMLInterpreter<S>::generate_object()
{
    // No tree to convert
    if (Super::root().is_null()) return false;

    // object already generated
    if (m_object) return false;

    // create root object
    m_object = new DataObject();

    return build_object(*m_object, Super::root());
}
#endif
