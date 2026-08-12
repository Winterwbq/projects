#include "waspcore/Interpreter.h"
#include "waspcore/utils.h"

namespace wasp
{
// Required by python bindings
Diagnostic::Diagnostic()
    : sl(1), el(1), sc(1), ec(1),
    msg (new std::stringstream()),
    interpreter(nullptr)
{
}
Diagnostic::Diagnostic(AbstractInterpreter* interp)
    : sl(1), el(1), sc(1), ec(1),
    msg (new std::stringstream()),
    interpreter(interp)
{
    wasp_require(interpreter);
}

Diagnostic::Diagnostic(const Diagnostic& orig)
    : sl(orig.sl), el(orig.el), sc(orig.sc), ec(orig.ec), f(orig.f),
    msg (new std::stringstream()),
    interpreter(orig.interpreter)
{
    wasp_require(interpreter);
    *msg << orig.msg->str();
}
Diagnostic::~Diagnostic()
{
    delete msg;
}

Diagnostic& Diagnostic::operator<< (location loc)
{
    wasp_check(interpreter);
    // Make a copy - the interpreter originating the location will go away
    f = *loc.begin.filename;
    sl = loc.begin.line;
    sc = loc.begin.column;
    el = loc.end.line;
    ec = loc.end.column;
    interpreter->error_stream() << loc;
    return *this;
}
Diagnostic& Diagnostic::operator<< (position start)
{
    wasp_check(interpreter);
    // Make a copy - the interpreter originating the location will go away
    f = *start.filename;
    sl = start.line;
    sc = start.column;
    el = sl;
    ec = sc;
    interpreter->error_stream() << start;
    return *this;
}

Diagnostic& Diagnostic::operator <<(std::ostream&(*os) (std::ostream&))
{
    wasp_check(msg);
    *msg << os;
    interpreter->error_stream() << os;
    return *this;
}
std::string Diagnostic::message() const
{
    wasp_check(msg);
    // trim front colon and spaces and end whitespace from returned message
    std::string msg_str = msg->str();
    msg_str.erase(0, msg_str.find_first_not_of(": "));
    msg_str.erase(msg_str.find_last_not_of(" \t\n\r") + 1);
    return msg_str;
}
std::string Diagnostic::str() const 
{
    std::stringstream s;
    // Always have start line and column
    s << f << ":" << sl << "." << sc;
    // Determine if there is additional range to convey
    auto end_col = 0 < ec ? ec - 1 : 0;
    if (sl < el)
      s << '-' << el << '.' << end_col;
    else if (sc < end_col)
      s << '-' << end_col;
    s << msg->str();
    return s.str();
}

std::string Diagnostic::filename() const
{
    return f;
}
int Diagnostic::start_line() const
{
    return sl;
}
int Diagnostic::start_column() const
{
    return sc;
}
int Diagnostic::end_line() const
{
    return el;
}
int Diagnostic::end_column() const
{
    return ec;
}

std::ostream& operator<<(std::ostream& stream, const std::vector<Diagnostic>& diagnostics)
{
    for (const auto& d : diagnostics)
    {
        stream << d.str();
    }
    return stream;
}


Diagnostic& AbstractInterpreter::error_diagnostic()
{
    // The following logic ensures that diagnostics are attached to the interpreter
    // with which the caller will be interacting
    if (document_parent()) return document_parent()->error_diagnostic();
    m_error_diagnostics.push_back(Diagnostic(&*this));
    return m_error_diagnostics.back();
}

NodeView AbstractInterpreter::find(size_t line , size_t column) const
{
    return wasp::findNodeUnderLineColumn(root(), line, column);
}

NodeView::NodeView(std::size_t node_index, wasp::AbstractInterpreter& nodes)
    : m_node_index(node_index), m_pool(&nodes)
{
}

NodeView::NodeView(const NodeView& orig)
    : m_node_index(orig.m_node_index), m_pool(orig.m_pool)
{
}

NodeView::~NodeView()
{
}

NodeView& NodeView::operator=(const NodeView& b)
{
    m_node_index = b.m_node_index;
    m_pool       = b.m_pool;
    return *this;
}

bool NodeView::operator==(const NodeView& b) const
{
    return m_pool == b.m_pool && m_node_index == b.m_node_index;
}

NodeView NodeView::parent() const
{
    NodeView view(m_pool->parent_node_index(m_node_index), *m_pool);
    return view;
}

bool NodeView::is_null() const
{
    return m_pool == nullptr || m_node_index == m_pool->size();
}

bool NodeView::has_parent() const
{
    return m_pool->has_parent(m_node_index);
}

bool NodeView::is_leaf() const
{
    return m_pool->is_leaf(m_node_index);
}

NodeView::Collection NodeView::non_decorative_children() const
{
    return wasp::non_decorative_children(*this);
}

NodeView NodeView::first_non_decorative_child_by_name(
    const std::string& name) const
{
    return wasp::first_non_decorative_child_by_name(*this, name);
}

size_t NodeView::non_decorative_children_count() const
{
    return wasp::non_decorative_children_count(*this);
}

std::string NodeView::data() const
{
    std::stringstream str;
    m_pool->data(m_node_index, str);
    return str.str();
}
void NodeView::set_data(const char* data)
{
    wasp_insist(is_leaf(), "Data assignment only allowed for leaf nodes!");
    m_pool->set_data(m_node_index, data);
}

std::string NodeView::path() const
{
    std::stringstream str;
    m_pool->node_path(m_node_index, str);
    return str.str();
}

void NodeView::paths(std::ostream& out) const
{
    m_pool->node_paths(m_node_index, out);
}

std::size_t NodeView::child_count() const
{
    return m_pool->child_count(m_node_index);
}
std::size_t  // return type
    NodeView::child_count_by_name(const std::string& name,
                                  std::size_t        limit) const
{
    std::size_t matching_named_child_count = 0;

    for (std::size_t i = 0, count = child_count(); i < count; ++i)
    {
        auto        child      = child_at(i);
        std::string child_name = child.name();
        if (child_name == name)
        {
            ++matching_named_child_count;
        }
        // limit of 0 is reserved as no limit
        if (limit != 0 && matching_named_child_count == limit)
            break;
    }
    return matching_named_child_count;
}
NodeView NodeView::child_at(std::size_t index) const
{
    auto child_node_pool_index = m_pool->child_at(m_node_index, index);
    return NodeView(child_node_pool_index, *m_pool);
}
NodeView::Collection  // return type
    NodeView::child_by_name(const std::string& name, std::size_t limit) const
{
    NodeView::Collection results;

    for (std::size_t i = 0, count = child_count(); i < count; ++i)
    {
        auto        child      = child_at(i);
        std::string child_name = child.name();
        if (child_name == name)
        {
            results.push_back(child);
        }
        // limit of 0 is reserved as no limit
        if (limit != 0 && results.size() == limit)
            break;
    }
    return results;
}
NodeView NodeView::first_child_by_name(const std::string& name) const
{
    for (std::size_t i = 0, count = child_count(); i < count; ++i)
    {
        auto child = child_at(i);
        if (name == child.name())
            return child;
    }
    return NodeView();  // null view
}

std::size_t NodeView::type() const
{
    return m_pool->type(m_node_index);
}

void NodeView::set_type(std::size_t node_type)
{
    m_pool->set_type(m_node_index, node_type);
}

std::size_t NodeView::token_type() const
{
    return m_pool->node_token_type(m_node_index);
}

const char* NodeView::name() const
{
    return m_pool->name(m_node_index);
}

std::size_t NodeView::line() const
{
    return m_pool->line(m_node_index);
}

std::size_t NodeView::column() const
{
    return m_pool->column(m_node_index);
}

std::size_t NodeView::last_line() const
{
    return m_pool->last_line(m_node_index);
}

std::size_t NodeView::last_column() const
{
    return m_pool->last_column(m_node_index);
}

bool NodeView::to_bool(bool* ok) const
{
    bool              result = false;
    std::stringstream str;
    str << std::boolalpha << data();
    str >> result;
    if (ok)
        *ok = !(str.bad() || str.fail());
    return result;
}

int NodeView::to_int(bool* ok) const
{
    int result = 0;
    to_type(result, data(), ok);
    return result;
}

double NodeView::to_double(bool* ok) const
{
    double result = 0.0;
    to_type(result, data(), ok);
    return result;
}

std::string NodeView::to_string(bool* ok) const
{
    std::string result;
    to_type(result, data(), ok);
    // trim front quotes
    result = wasp::strip_quotes(result);
    return result;
}

std::string NodeView::last_as_string(bool* ok) const
{
    return wasp::last_as_string(*this, ok);
}

}  // end of namespace
