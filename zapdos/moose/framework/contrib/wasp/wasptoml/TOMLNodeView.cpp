#include "wasptoml/TOMLNodeView.h"
#include "waspcore/utils.h"

namespace wasp
{
TOMLNodeView::TOMLNodeView(std::size_t                node_index,
                               AbstractInterpreter& data)
    : m_node_index(node_index), m_pool(&data)
{
}
TOMLNodeView::TOMLNodeView(const TOMLNodeView& orig)
    : m_node_index(orig.m_node_index), m_pool(orig.m_pool)
{
}

TOMLNodeView::~TOMLNodeView()
{
}

TOMLNodeView& TOMLNodeView::operator=(const TOMLNodeView& b)
{
    m_node_index = b.node_index();
    m_pool       = b.node_pool();
    return *this;
}

bool TOMLNodeView::operator==(const TOMLNodeView& b) const
{
    return m_pool == b.m_pool && m_node_index == b.m_node_index;
}

bool TOMLNodeView::operator<(const TOMLNodeView& b) const
{
    return m_node_index < b.m_node_index;
}

TOMLNodeView TOMLNodeView::parent() const
{
    return wasp::fe_parent<TOMLNodeView, AbstractInterpreter>(*this);
}

bool TOMLNodeView::has_parent() const
{
    return wasp::fe_has_parent(*this);
}

bool TOMLNodeView::is_leaf() const
{
    NodeView view(node_index(), *node_pool());
    return view.is_leaf();
}

bool TOMLNodeView::is_decorative() const
{
    auto t = type();
    switch (t)
    {
        case wasp::ASSIGN:
        case wasp::COMMENT:
        case wasp::WASP_COMMA:
        case wasp::SEPARATOR: // .
        case wasp::LBRACKET:  // [
        case wasp::RBRACKET:  // ]
        case wasp::LBRACE:  // {
        case wasp::RBRACE:  // }
        case wasp::DLBRACKET:  // [[]
        case wasp::DRBRACKET:  // ]]
            return true;
    }
    return false;
}

bool TOMLNodeView::is_declarator() const
{
    return type() == wasp::DECL;
}

bool TOMLNodeView::is_terminator() const
{
    switch (type())
    {
        case wasp::RBRACKET:
        case wasp::RBRACE:
            return true;
        default:
            return false;
    }
}

TOMLNodeView::Collection TOMLNodeView::non_decorative_children() const
{
    return wasp::fe_non_decorative_children(*this);
}

TOMLNodeView TOMLNodeView::first_non_decorative_child_by_name(
    const std::string& name) const
{
    return wasp::fe_first_non_decorative_child_by_name(*this, name);
}

size_t TOMLNodeView::non_decorative_children_count() const
{
    return wasp::fe_non_decorative_children_count(*this);
}

std::string TOMLNodeView::data() const
{
    std::stringstream str;
    m_pool->data(m_node_index, str);
    return str.str();
}

void TOMLNodeView::set_data(const char* data)
{
    NodeView view(node_index(), *node_pool());
    view.set_data(data);
}

std::string TOMLNodeView::path() const
{
    return wasp::node_path(*this);
}

void TOMLNodeView::paths(std::ostream& out) const
{
    wasp::node_paths(*this, out);
}

std::size_t TOMLNodeView::child_count() const
{
    return wasp::fe_child_count(*this);
}
std::size_t  // return type
    TOMLNodeView::child_count_by_name(const std::string& name,
                                        std::size_t        limit) const
{
    return wasp::fe_child_count_by_name(*this, name, limit);
}

TOMLNodeView TOMLNodeView::child_at(std::size_t index) const
{
    return wasp::fe_child_at(*this, index);
}
TOMLNodeView::Collection  // return type
    TOMLNodeView::child_by_name(const std::string& name,
                                  std::size_t        limit) const
{
    return wasp::fe_child_by_name(*this, name, limit);
}
TOMLNodeView  // return type
    TOMLNodeView::first_child_by_name(const std::string& name) const
{
    return wasp::fe_first_child_by_name(*this, name);
}

std::size_t TOMLNodeView::type() const
{
    return m_pool->type(m_node_index);
}
std::size_t TOMLNodeView::token_type() const
{
    return m_pool->node_token_type(m_node_index);
}

void TOMLNodeView::set_type(std::size_t node_type)
{
    m_pool->set_type(m_node_index, node_type);
}

const char* TOMLNodeView::name() const
{
    return m_pool->name(m_node_index);
}

std::size_t TOMLNodeView::line() const
{
    return m_pool->line(m_node_index);
}

std::size_t TOMLNodeView::column() const
{
    return m_pool->column(m_node_index);
}

std::size_t TOMLNodeView::last_line() const
{
    return m_pool->last_line(m_node_index);
}

std::size_t TOMLNodeView::last_column() const
{
    return m_pool->last_column(m_node_index);
}

bool TOMLNodeView::to_bool(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_bool(ok);
}

int TOMLNodeView::to_int(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_int(ok);
}

double TOMLNodeView::to_double(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_double(ok);
}

std::string TOMLNodeView::to_string(bool* ok) const
{
    NodeView view(value_node_index(), *node_pool());
    return view.to_string(ok);
}

std::string TOMLNodeView::last_as_string(bool* ok) const
{
    return wasp::last_as_string(*this, ok);
}

size_t TOMLNodeView::value_node_index() const
{
    /// TODO - could push this lower to NodeView ?
    if (type() == wasp::KEYED_VALUE && child_count() > 0)
    {
        return child_at(child_count() - 1).node_index();
    }
    return node_index();
}
}  // end of namespace wasp
