#include "waspmcnpi/Model.h"
#include "waspcore/wasp_bug.h"
#include "waspcore/wasp_math.h"
#include "waspcore/Object.h"
#include "waspmcnpi/mcnp_node.h"

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <string>
#include <stdexcept>

namespace wasp
{
namespace mcnpi
{
    // Basically build everything BUT cells until cell importances are figured out
    bool Model::build_limited(const NodeView& document_root, std::ostream& error)
    {
        bool success = true;

        // process transformations
        success &= build_transforms(document_root, error);

        // process surfaces
        success &= build_surfaces(document_root, error);

        // process materials
        success &= build_materials(document_root, error);

        return success;

    }

    bool Model::build(const NodeView& document_root, std::ostream& error)
    {
        bool success = true;

        // process transformations
        success &= build_transforms(document_root, error);

        // process surfaces
        success &= build_surfaces(document_root, error);

        // process materials
        success &= build_materials(document_root, error);

        // process cells
        success &= build_cells(document_root, error);
        return success;
    }

    void Model::describe_cells(std::ostream& out) const
    {
        for (const Cell& c : m_cells)
        {
            out << "cell " << c.id << " mat: " << c.mat_id << " rho: " << c.rho
                << " csg: ";
            describe_cell_csg(c.csg_index, out); out << std::endl;
        }
    }

    void Model::describe_cell_csg(size_t csg_index, std::ostream& out) const
    {
        wasp_require(csg_index < m_cell_csg_nodes.size());
        const auto& csg_node = m_cell_csg_nodes.at(csg_index);
        wasp_check(csg_node.is_null() == false);
        out << "(";
        // Should only see these operators in practice
        if (csg_node.is_left_node())
        {
            if (csg_node.op == CSG_OP::Complement) out << "#";
            wasp_check(csg_node.left < m_cell_csg_nodes.size());
            describe_cell_csg(csg_node.left, out);
        } 
        else
        {
            wasp_check(m_surfaces.size() > csg_node.left);
            if (csg_node.op == CSG_OP::Complement) out << "#";
            out << (csg_node.left_sense==Surface_Sense::Inside ? "-":"+") << m_surfaces[csg_node.left].id;
        }
        
        
        // If binary operation, print right side
        if (csg_node.op == CSG_OP::Union || csg_node.op == CSG_OP::Intersection)
        {
            out << "PU^#LE"[int(csg_node.op)];
            if (csg_node.is_right_node())
            {
                describe_cell_csg(csg_node.right, out);
            }
            else 
            {
                wasp_check(m_surfaces.size() > csg_node.right);
                out << (csg_node.right_sense==Surface_Sense::Inside ? "-":"+") << m_surfaces[csg_node.right].id;
            }
        }
        out << ")";
    }

    void Model::describe_materials_json(std::ostream& out) const
    {
        DataObject root;
        root["mixtures"] = DataArray();
        auto& mixtures = root["mixtures"].as_array();
        for (const auto& c : m_cells)
        {

            // skip void
            if (c.mat_id == 0) continue;

            auto material_index = m_material_id_index.at(c.mat_id);
            const auto& m = m_materials.at(material_index);
            // Store the ZAID : adens list
            DataObject mixture;
            mixture["mixture"] = m.id;
            mixture["cell"] = c.id;
            mixture["density"] = c.rho;
            mixture["nuclides"] = DataArray();
            auto& nuclides = mixture["nuclides"].as_array();
            for (std::size_t idx=0; idx < m.zaid_count; ++idx)
            {
                DataObject nuclide;
                Material_Zaid_Entry mza = m_material_zaids[idx+m.zaid_index];
                nuclide["zaid"] = mza.zaid;
                nuclide["adens"] = c.nuclide_densities[idx];
                // could add symbol (h1), if data was available 
                nuclides.push_back(nuclide);
            }
            
            // Store any associated comment metadata
            DataArray m_metadata;
            for (const std::string& md : m.metadata) 
            {
                m_metadata.push_back(md);
            }
            if(!m_metadata.empty())
            {
                mixture["metadata"] = DataObject();
                mixture["metadata"]["data"] = m_metadata;
                mixture["metadata"]["column"] = (int) m.column;
                mixture["metadata"]["line"] = (int) m.line;
            }
            mixtures.push_back(mixture); 
        }
        bool success = root.format_json(out, 2);
        out << std::endl;
    }

    bool Model::build_transforms(const NodeView& document_root, std::ostream& error)
    {
        bool success = true;
        const NodeView::Collection& nodes = document_root.child_by_name("trcl");        
        return success;
    }
    
    bool Model::build_surfaces(const NodeView& document_root, std::ostream& error)
    {
        
        using Surface_Build_Method = bool (Model::*)(const NodeView&, std::ostream& );
        static const std::unordered_map<std::string, Surface_Build_Method> surface_builders
        {
            {"sph",     &Model::build_sphere},
            {"sq",      &Model::build_simple_quadric},
            {"gq",      &Model::build_general_quadric},
            {"xcyl",    &Model::build_cylinder},
            {"ycyl",    &Model::build_cylinder},
            {"zcyl",    &Model::build_cylinder},
            {"cyl",     &Model::build_pcylinder},
            {"cone",    &Model::build_cone},
            {"xcone",   &Model::build_xyzcone},
            {"ycone",   &Model::build_xyzcone},
            {"zcone",   &Model::build_xyzcone},
            {"xpcone",  &Model::build_pcone},
            {"ypcone",  &Model::build_pcone},
            {"zpcone",  &Model::build_pcone},
            {"xtorus",  &Model::build_torus},
            {"ytorus",  &Model::build_torus},
            {"ztorus",  &Model::build_torus},
            {"xpl",     &Model::build_plane},
            {"ypl",     &Model::build_plane},
            {"zpl",     &Model::build_plane},
            {"pl",      &Model::build_abcplane},
            {"xpts",    &Model::build_points},
            {"ypts",    &Model::build_points},
            {"zpts",    &Model::build_points},
            {"box",     &Model::build_box},
            {"hex",     &Model::build_hexagon},
            {"rppiped", &Model::build_rectangular_parallelepiped},
            {"rrectcyl",&Model::build_right_rectangular_cylinder},
            {"relipcyl",&Model::build_right_eliptical_cylinder},
            {"plpts",   &Model::build_plane_points},
        };
        bool success = true;
        const NodeView::Collection& nodes = document_root.child_by_name("surface");
        for (auto surface_node : nodes)
        {
            size_t id = 0;
            size_t transform_id = 0;
            size_t periodic_surface_id = 0;
            size_t surface_index = m_surfaces.size();
            for(auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
            {
                auto child = itr.get();
                std::string name = child.name();
                if (name == "id")
                {
                    std::string sid = child.data();
                    auto reflection_index = sid.find_first_of("*");
                    if (reflection_index != std::string::npos)
                    {
                        // TODO capture reflecting boundary condition
                        sid[reflection_index] = ' '; // Remove the asterisk
                    }
                    try{
                        id = ::std::stoi(sid);
                    }
                    catch(const std::exception& e)
                    {
                        error << "Error - line: " << child.line() <<", column: " << child.column() 
                        << " failed to extract surface id from '" << sid << "'!" << std::endl;
                        success &=false;
                        return success;
                    }
                }
                else if (name == "periodic")
                {
                    // Always specified as a negative id - convert accordingly
                    try
                    {
                        periodic_surface_id = -std::stoi(child.data());
                    }
                    catch(const std::exception& e)
                    {
                        error << "Error - line: " << child.line()
                              << ", column: " << child.column()
                              << " failed to extract periodic surface id from '"
                              << child.data() << "'!" << std::endl;
                        success &=false;
                        return success;
                    }
                }
                else if (name == "transform")
                {
                    try
                    {
                        transform_id = ::std::stoi(child.data());
                    }
                    catch(const std::exception& e)
                    {
                        error << "Error - line: " << child.line()
                              << ", column: " << child.column()
                              << " failed to extract transform id from '"
                              << child.data() << "'!" << std::endl;
                        success &=false;
                        return success;
                    }
                }
                else if (name == "comment" || name == "LC")
                {
                    continue; // skip comments
                }
                else 
                {
                    // assume it is the surface
                    auto build_iter = surface_builders.find(name);
                    if (build_iter == surface_builders.end())
                    {
                        error << "Error - line: " << child.line()
                              << ", column: " << child.column() << " named "
                              << name
                              << " does not have a known surface builder!"
                              << std::endl
                              << surface_node.data() << std::endl;
                        success &=false;
                        return success;
                    }

                    // Invoke surface build method
                    Surface_Build_Method sbp = build_iter->second;
                    wasp_check(build_iter->second);
                    success &= (this->*sbp)(child, error);
                }
            } // end of loop of surface children (id, coefficents, comments)

            wasp_ensure(id != 0);
            m_surfaces[surface_index].id = id;            
            m_surfaces[surface_index].line = surface_node.line();
            m_surfaces[surface_index].transform_id = transform_id;
            m_surfaces[surface_index].periodic_surface_id = periodic_surface_id;
        }
        // Sort surfaces allow log(n) surface lookups as a function of surface ID
        auto binary_predicate = [](const Surface& a, const Surface& b) {return a.id < b.id;};
        std::sort(m_surfaces.begin(), m_surfaces.end(), binary_predicate);
        

        // Ensure no duplicate surface ids
        for (size_t i = m_surfaces.size()-1; m_surfaces.size() > 1 && i > 0; --i)
        {
            const auto& s1 = m_surfaces[i];
            const auto& s2 = m_surfaces[i-1];
            if (s1.id != s2.id) continue;

            error << "Error - line: " << s1.line << " surface id (" << s1.id
                  << ") collides with surface defined at line " << s2.line
                  << "!" << std::endl;
            m_surfaces.erase(m_surfaces.begin()+i);
            success &=false;
        }
        return success;
    }

    bool Model::build_cells(const NodeView& document_root, std::ostream& error)
    {
        bool success = true;
        const NodeView::Collection& nodes = document_root.child_by_name("cell");
        for( const auto& node : nodes)
        {
            success &= build_cell(node, error);
        }
        return success;
    }

    bool Model::build_cell(const NodeView& cell_node, std::ostream& error)
    {
        wasp_require(std::strcmp(cell_node.name(),"cell") == 0);
        bool success = true;
        Cell c;
        NodeView material_node;
        for (auto itr = cell_node.begin(); itr != cell_node.end(); itr.next())
        {
            // process id, material, rho, geom, and params
            auto child = itr.get();
            std::string name = child.name();
            if (name == "id")
            {
                c.id = std::stoi(child.data());
            }
            else if (name == "material")
            {
                c.mat_id = std::stoi(child.data());
                material_node = child;
            }
            else if (name == "rho")
            {
                c.rho = std::stod(child.data());
            }
            else if (name == "geom")
            {
                success &= build_cell_geom(c, child, error);
            }
            else if (name == "params")
            {
                success &= build_cell_params(c, child, error);
            }
            else if (name == "like")
            {
                success &= build_cell_like(c, child, error);
            }
            else if (name == "comment" || name == "LC")
            {
                continue; // skip comments
            }
            else 
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " is an unexpected cell component!" << std::endl;
                success &= false;
            }
        }
        // Regardless of success, as long as there is an id
        // we capture the processed cell
        if (c.id != 0)
        {
            if (c.mat_id > 0) 
                    success &= build_cell_material(c, material_node.is_null() ? cell_node : material_node, error);
            m_cells.push_back(c);
        }
        return success;
    }

    /**
     * @brief Convert an infix expression to postfix 
     * 
     * @param cell_geom_node the parent of the infix expression
     * @param postfix the collection of nodes describing the postfix expression (null node describes intersection operator)
     * @param error expression errors are reported here
     * @return true iff no errors occur
     * @return false if an error occurs. Error stream contains the error
     */
    bool Model::convert_infix_postfix(const NodeView& cell_geom_node, NodeView::Collection& postfix, std::ostream& error)
    {

        NodeView::Collection operator_stack;
        std::vector<size_t> operand_indices;

        auto is_lparen = [](const NodeView& a) -> bool {
            return !a.is_null() && a.type() == mcnpi::NODE::LPAREN;
        };
        auto has_explicit_op = [](const NodeView& cell_geom_node,
                                  size_t          last_pf_index,
                                  size_t          current_index) -> bool {
            wasp_require(last_pf_index < current_index);
            for (size_t i = last_pf_index+1; i < current_index; i++)
            {   
                auto child = cell_geom_node.child_at(i);
                auto ctype = child.type();
                if (ctype == mcnpi::NODE::LPAREN ||
                    ctype == mcnpi::NODE::UNION ||
                    ctype == mcnpi::NODE::COMPLEMENT)
                {
                    return true;
                }
            }
            return false;
        };

        auto pop_push = [](NodeView::Collection& operator_stack,
                           NodeView::Collection& postfix,
                           const NodeView&       child) {
            auto higher_precedence = [](const NodeView& a,
                                        const NodeView& b) -> bool {
                CSG_OP a_op = a.is_null() ? CSG_OP::Intersection
                              : a.type() == mcnpi::NODE::UNION ? CSG_OP::Union
                              : a.type() == mcnpi::NODE::COMPLEMENT
                                  ? CSG_OP::Complement
                                  : CSG_OP::Parenthesis;
                CSG_OP b_op = b.is_null() ? CSG_OP::Intersection
                              : b.type() == mcnpi::NODE::UNION ? CSG_OP::Union
                              : b.type() == mcnpi::NODE::COMPLEMENT
                                  ? CSG_OP::Complement
                                  : CSG_OP::Parenthesis;

                return a_op > b_op;
            };
            // pop until proper precedence or empty
            while (!operator_stack.empty() &&
                   !higher_precedence(child, operator_stack.back()))
            {
                postfix.push_back(operator_stack.back());
                operator_stack.pop_back();
            }
            if (operator_stack.empty())
            {
                operator_stack.push_back(child);
            
            }
            else if (higher_precedence(child, operator_stack.back()))
            {
                // child is higher precedence so push to operator stack
                operator_stack.push_back(child);
            }
        };
        for (size_t i = 0; i < cell_geom_node.child_count(); ++i)
        {   
            auto child = cell_geom_node.child_at(i);
            // skip comments and line continuations
            if (child.type() == mcnpi::NODE::COMMENT ||
                child.type() == mcnpi::NODE::LINE_CONTINUATION)
                continue;

            // Right paren dictates we pop operator stack and push operators onto postfix expression
            if (child.type() == mcnpi::NODE::RPAREN)
            {
                while (!operator_stack.empty() &&
                       !is_lparen(operator_stack.back()))
                {
                    // remember, null node is placeholder for Intersection
                    postfix.push_back(operator_stack.back());
                    operator_stack.pop_back();
                }
                if (operator_stack.empty() || !is_lparen(operator_stack.back()))
                {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column()
                          << " right parenthesis is missing a matching left "
                             "parenthis!"
                          << std::endl;
                    return false;
                }
                // Remove lparen to complete the parethetical operation
                operator_stack.pop_back();
                continue; // move to next node
            }
            // always push opening parenthetical
            if (child.type() == mcnpi::NODE::LPAREN)
            {
                // if prior operand  has no explicit operator separating this node/operand
                // then it is deamed to have an intersection operator that needs to be captured
                if (!operand_indices.empty() && !has_explicit_op(cell_geom_node, operand_indices.back(), i))
                {
                    pop_push(operator_stack, postfix, NodeView());
                }
                operator_stack.push_back(child);
                continue;
            }
            // because complement is an unary it could be predicated by an implicit intersection... check
            else if (child.type() == mcnpi::NODE::COMPLEMENT)
            {
                // if prior operand  has no explicit operator separating this node/operand
                // then it is deamed to have an intersection operator that needs to be captured
                if (!operand_indices.empty() &&
                    !has_explicit_op(cell_geom_node, operand_indices.back(), i))
                {
                    pop_push(operator_stack, postfix, NodeView());
                }
            }
            bool is_explicit = child.type() == mcnpi::NODE::UNION ||
                               child.type() == mcnpi::NODE::COMPLEMENT;
            // not an explicit operator - because intersection operator is implicit (no explicit node) 
            if (!is_explicit)
            {
                wasp_require(std::strcmp(child.name(),"id")==0);
                // if prior operand  has no explicit operator separating this node/operand
                // then it is deamed to have an intersection operator that needs to be captured
                if (!operand_indices.empty() &&
                    !has_explicit_op(cell_geom_node, operand_indices.back(), i))
                {
                    pop_push(operator_stack, postfix, NodeView());
                }
                postfix.push_back(child); // capture operand in postfix
                operand_indices.push_back(i); // capture index of last operand
                continue;
            }
            // only UNION or COMPLEMENT explicit operator remain
            wasp_check(is_explicit);
            pop_push(operator_stack, postfix, child);
        }

        while (!operator_stack.empty())
        {
            postfix.push_back(operator_stack.back());
            operator_stack.pop_back();
        }
        return true;
    }

    bool Model::build_cell_geom(Cell&           c,
                                const NodeView& cell_geom_node,
                                std::ostream&   error)
    {
        wasp_require(std::strcmp(cell_geom_node.name(),"geom")==0);
        
        // Process id and union, intersection, and complement operators
        // We expect the csg tree to be flat list of children, recursively process 
        size_t start = 0;
        NodeView::Collection postfix;
        if (!convert_infix_postfix(cell_geom_node, postfix, error))
        {
            return false;
        }
        size_t csg_index = build_csg_tree(postfix, error);

        if (csg_index == m_cell_csg_nodes.size()) return false;
        c.csg_index = csg_index;
        return true;
    }

    bool Model::build_cell_like(Cell& c, const NodeView& cell_like_node, std::ostream& error)
    {
        wasp_require(std::strcmp(cell_like_node.name(),"like")==0);
        for (auto itr = cell_like_node.begin(); itr != cell_like_node.end(); itr.next())
        {
            auto child = itr.get();
            const std::string& name = child.name();
            if (name == "cell")
            {
                size_t cell_id = std::stoi(child.data());
                size_t cell_index = this->find_cell_index(cell_id);
                if (cell_index == m_cells.size())
                {
                    error << "***Error - line: " << child.line()
                          << ", column: " << child.column()
                          << " does not reference a previously defined cell "
                             "definition!"
                          << std::endl;
                    return false;
                }
                const auto& like_cell = m_cells[cell_index];
                c.csg_index = like_cell.csg_index;
                c.mat_id = like_cell.mat_id;
                c.rho = like_cell.rho;
                c.trans_index = like_cell.trans_index;
                c.lattice_type = like_cell.lattice_type;
                c.fill_index = like_cell.fill_index;
                c.universe_id = like_cell.universe_id;
            }
            else if (name == "params")
            {
                if (!build_cell_params(c, child, error))
                {
                    return false;
                }
            }
            else if (name == "decl" || name == "comment" || name == "LC")
            {
                continue; // skip declarator, comment, and line continuation
            }
        }
        return true;
    }

    bool Model::build_cell_params(Cell& c, const NodeView& params_node, std::ostream& error)
    {
        wasp_require(std::strcmp(params_node.name(), "params") == 0);
        for(auto itr = params_node.begin(); itr != params_node.end(); itr.next())
        {
            auto child = itr.get();
            const std::string& name = child.name();
            if (name == "universe")
            {
                // u [=] id, -id indicates distance to boundary of higher-level cell can be omitted
                // embedded mesh universe ids start with an 'e'
                const std::string& data = child.last_as_string();
                if (data.front() == 'e') wasp_not_implemented("mesh-based universes");
                c.universe_id = std::abs(std::stoi(data));
            }
            else if (name == "rho")
            {
                const std::string& data = child.last_as_string();
                c.rho = std::stod(data);
            }
            else if (name == "mat")
            {
                const std::string& data = child.last_as_string();
                c.mat_id = std::abs(std::stoi(data));
            }
            else if (name == "comment" || name == "LC")
            {
                continue; //  comment, and line continuation
            }
            else
            {
                // TODO - implement these remaining items, but don't fail
                // Allow the utility in use to work in supported scenarios
                // wasp_not_implemented(name << " is not supported");
            }
        }
        return true;
    }

    /**
     * Create cell-specific material nuclide atomic densities (g/b-cm)
     * @param Cell c - the cell from which the density ($\rho$, or $D_T$) and material inventory (zaids mapped to $f_i$ or $w_i$) 
     * @return true, iff the cell material's nuclide atomic densities are successfully calculated
     * Fail scenarios involve, referenced material not existing/accessible, 
     * inconsistent nuclide inventory specification (mixed weight and atom fraction),
     * and missing molar mass library. 
     * Note: The cell.nuclide_zaids and nuclide_densities will be populated.
     */
    bool Model::build_cell_material(Cell& c, const NodeView& cell_mixture_node, std::ostream& error)
    {
        wasp_require(c.mat_id > 0);
        wasp_require(c.rho != 0);

        // obtain the material information. 
        if (m_material_id_index.find(c.mat_id) == m_material_id_index.end())
        {
            error << "*** Error - line: " << cell_mixture_node.line()
                << ", column: " << cell_mixture_node.column()
                << " references material " << c.mat_id << " which is not defined in the problem!"
                << std::endl;
            return false;
        }

        // Determine if cell material density is atomic or mass
        bool is_cell_mass_density = c.rho < 0; 

        size_t material_index = m_material_id_index[c.mat_id];
        
        const Material& material = m_materials[material_index];
        wasp_check(material.id == c.mat_id);

        // May reference a material that doesn't have inventory correctly specified
        if (material.zaid_count == 0)
        {
            error << "*** Error - line: " << cell_mixture_node.line()
                << ", column: " << cell_mixture_node.column()
                << " references material " << c.mat_id << " which has no nuclide inventory captured!"
                << std::endl;
            return false;      
        }

        c.nuclide_zaids.resize(material.zaid_count); 
        std::vector<double> zaid_values(material.zaid_count);
        std::vector<double> molar_masses(material.zaid_count);
        size_t zaid_index_end = material.zaid_index+material.zaid_count;

        // Use first zaid value as indicator of weight/mass or atom fraction input
        bool is_nuclide_mass_fraction = m_material_zaids[material.zaid_index].value < 0;

        // We need zaid molar masses if involving material atom fractions or cell atomic density
        if ((is_nuclide_mass_fraction || is_cell_mass_density)  && m_zaid_mass_map == nullptr)
        {
             error << "*** Internal Error - line: " << cell_mixture_node.line()
                << ", column: " << cell_mixture_node.column()
                << " cannot be converted because the required molar mass data library has not be set!"
                << std::endl;
            return false;
        }
        
        for (std::size_t idx=0; idx < material.zaid_count; ++idx)
        {
            const Material_Zaid_Entry mza = m_material_zaids[idx+material.zaid_index];
            if (is_nuclide_mass_fraction)
            {
                // weight fractions must uniformly be specified (all < 0)
                wasp_check (mza.value < 0);
                // capture positive value
                zaid_values[idx] = -mza.value;
            }
            else {
                wasp_check(mza.value > 0);
                zaid_values[idx] = mza.value;
            }
            c.nuclide_zaids[idx] = mza.zaid;

            // capture molar mass for given zaid, if provided
            if (m_zaid_mass_map)
            {
                wasp_insist(m_zaid_mass_map->find(mza.zaid) != m_zaid_mass_map->end(), "zaid mass map is missing " << mza.zaid << "!");
                molar_masses[idx] = m_zaid_mass_map->at(mza.zaid);
            }
        }
        

        if (is_nuclide_mass_fraction)
        {
            if (is_cell_mass_density) c.nuclide_densities = compute_di_from_weight_fraction_mass_density(zaid_values, c.rho);
            else c.nuclide_densities = compute_di_from_weight_fraction_atomic_density(zaid_values, molar_masses, c.rho);
        }
        else // atom fraction 
        {
            if (is_cell_mass_density) c.nuclide_densities = compute_di_from_atom_fraction_mass_density(zaid_values, molar_masses, c.rho);
            else c.nuclide_densities = compute_di_from_atom_fraction_atomic_density(zaid_values, c.rho);
        }

        // Ensure cell density is assigned g/cc, if molar masses provided
        if (m_zaid_mass_map)
        {
            c.rho = compute_mass_density_from_atoms_bcm(c.nuclide_densities, molar_masses);
        }
        return true;
    }
    bool Model::get_index(const NodeView& node,
                          size_t&         s_index,
                          size_t&         csg_index,
                          std::ostream&   error) const
    {
        wasp_require(node.is_null() == false);
        int iid = std::stoi(node.data());
        size_t aiid = std::abs(iid);
        s_index = this->surface_index(aiid);
        if (s_index == m_surfaces.size())
        {
            // Because get_index is used during construction cells are still in
            // user-provided order (potentially not sorted)... use find_cell_index instead of cell_index
            size_t c_index = this->find_cell_index(aiid);
            if (c_index == m_cells.size())
            {
                error << "***Error - line: " << node.line()
                      << ", column: " << node.column()
                      << ", unable to find surface or cell with identifier '"
                      << aiid << "'!" << std::endl;
                return false;
            }
            wasp_check(m_cells[c_index].csg_index < m_cell_csg_nodes.size());
            csg_index = m_cells[c_index].csg_index;
        }
        return true;
    }

    void print_postfix(NodeView::Collection& p)
    {
        for (auto n : p)
        {
            if (n.is_null())
            {
                std::cout << "^"; continue;
            }
            std::cout <<n.name() <<"/"<<n.data();
        }
        std::cout << std::endl;
    }

    size_t Model::build_csg_tree(NodeView::Collection& postfix,
                                 std::ostream&         error)
    {
        wasp_require(postfix.empty() == false);
        // singlularity of lone id
        if (postfix.size() == 1)
        {
            auto child = postfix.back();
            // must be an actual leaf node
            wasp_check(!child.is_null() && std::strcmp(child.name(), "id") == 0);
            // extract surface id
            int iid = std::stoi(child.data());
            size_t aiid = std::abs(iid);
            size_t left_surface_index = this->surface_index(aiid);
            if (left_surface_index == m_surfaces.size())
            {
                error << "***Error - line: " << child.line()
                      << ", column: " << child.column()
                      << ", unable to find surface with identifier '" << aiid
                      << "'!" << std::endl;
                return false;
            }
            m_cell_csg_nodes.resize(m_cell_csg_nodes.size()+1);
            auto& inode = m_cell_csg_nodes.back();            
            inode.left = left_surface_index;
            inode.left_sense = iid < 0 ? Surface_Sense::Inside : Surface_Sense::Outside;
            inode.op = CSG_OP::Leaf;
            return m_cell_csg_nodes.size() - 1;
        }

        NodeView::Collection stack; // null node represents last m_cell_csg_nodes member

        // iterator postfix expression and perform the following csg tree construction algorithm
        // 
        for(size_t i = 0; i < postfix.size(); ++i)
        {
            auto node = postfix[i];

            CSG_OP node_op = node.is_null() ? CSG_OP::Intersection
                             : node.type() == mcnpi::NODE::UNION ? CSG_OP::Union
                             : node.type() == mcnpi::NODE::COMPLEMENT
                                 ? CSG_OP::Complement
                                 : CSG_OP::Leaf;
            // Surface/cell reference... push to stack for incorporation into future operator node 
            if (node_op == CSG_OP::Leaf)
            {
                wasp_check(std::strcmp(node.name(),"id")==0);
                stack.push_back(node);
                continue;
            }
            // dealing with operator node            
            m_cell_csg_nodes.resize(m_cell_csg_nodes.size()+1);
            auto& inode = m_cell_csg_nodes.back();
            inode.op = node_op;

            size_t left_side_csg_offset = 2; 
            // process right side if binary operator
            if (node_op != CSG_OP::Complement)
            {
                wasp_check(node_op == CSG_OP::Union || node_op == CSG_OP::Intersection);
                wasp_check(stack.empty() == false);
                auto rnode = stack.back();
                // Null node on the stack is placeholder for csg node
                if (rnode.is_null())
                {
                    // is sub-tree csg - 2nd to last csg created (last is current node under construction)
                    wasp_check(m_cell_csg_nodes.size() > 1);
                    inode.right = m_cell_csg_nodes.size() - 2;
                    ++left_side_csg_offset; // need to look farther back if left is also a csg node
                }
                else 
                {
                    // a surface or cell (cell is represented by a csg_index)
                    size_t sindex = m_surfaces.size();
                    size_t csg_index = m_cell_csg_nodes.size();
                    if (!get_index(rnode, sindex, csg_index, error))
                    {
                        return m_cell_csg_nodes.size(); // invalid, error state
                    }
                    wasp_check(sindex != m_surfaces.size() || csg_index != m_cell_csg_nodes.size());
                    if (sindex != m_surfaces.size())
                    {
                        inode.right = sindex;
                        inode.right_sense = rnode.data()[0] == '-'
                                                ? Surface_Sense::Inside
                                                : Surface_Sense::Outside;
                    }
                    else  // csg
                    {
                        inode.right = csg_index;
                    }
                }
                stack.pop_back();
            }

            wasp_check(stack.empty()==false);
            auto lnode = stack.back();
            // Null node on the stack is placeholder for csg node
            if (lnode.is_null())
            {
                // is sub-tree csg - 2nd to last csg created (last is current node under construction)
                wasp_check(m_cell_csg_nodes.size() > 1);
                inode.left = m_cell_csg_nodes.size() - left_side_csg_offset;
                wasp_check(inode.left < m_cell_csg_nodes.size());
            }
            else 
            {
                // a surface or cell (cell is represented by a csg_index)
                size_t sindex = m_surfaces.size();
                size_t csg_index = m_cell_csg_nodes.size();
                if (!get_index(lnode, sindex, csg_index, error))
                {
                    return m_cell_csg_nodes.size(); // invalid, error state
                }
                wasp_check(sindex != m_surfaces.size() || csg_index != m_cell_csg_nodes.size());
                // is surface reference
                if (sindex != m_surfaces.size())
                {
                    inode.left = sindex;
                    inode.left_sense = lnode.data()[0] == '-' ? Surface_Sense::Inside : Surface_Sense::Outside;
                }
                else  // csg reference
                { 
                    inode.left = csg_index;
                }
            }
            stack.pop_back(); // left node from stack
            // push null node to represent the resulting csg node that 
            // took the place of the recently popped expression members
            stack.push_back(NodeView()); 
            wasp_ensure(!m_cell_csg_nodes.back().is_null());
        }
        wasp_ensure(m_cell_csg_nodes.size() > 0);
        wasp_ensure(!m_cell_csg_nodes.back().is_null());
        return m_cell_csg_nodes.size() - 1; // return newly constructed csg node index
    }
    bool Model::build_materials(const NodeView& document_root, std::ostream& error)
    {
        bool success = true;
        const NodeView::Collection& all_nodes = document_root.non_decorative_children();
        const NodeView::Collection& mat_nodes = document_root.child_by_name("material");
        const NodeView::Collection& surface_nodes = document_root.child_by_name("surface");

        if (surface_nodes.empty()) return false;
        // Search iterator for locating where our previous material node is on the stack
        auto iter = all_nodes.cbegin();        
        // Position the search iterator to begin after the last surface card 
        // (prior to any data cards)
        while (iter != all_nodes.cend())
        {
             if (iter->equal(surface_nodes.back()))
             {
                 break;
             }
             ++iter;
        }
     
        for (auto mnode : mat_nodes)
        {
            success &= build_material(mnode, error);
            if(success)
            {
                // Grab up any comment nodes preceding our current material node
                while(!(iter->equal(mnode) || (iter == all_nodes.end())))
                {  
                    if(iter->type() == mcnpi::NODE::COMMENT)
                    { 
                        bool got_comment = true;
                        m_materials.back().metadata.push_back(iter->to_string(&success));
                        wasp_check(got_comment);

                        // Set the location of the material based on the first observed preceeding comment
                        if (m_materials.back().line == 0)
                        {
                            m_materials.back().line = iter->line();
                            m_materials.back().column = iter->column();
                        }
                    }
                    ++iter;
                } 
            }
        }
        return success;
    }

    bool Model::build_material(const NodeView& material_node, std::ostream& error)
    {
        wasp_require(std::strcmp(material_node.name(), "material") == 0);
        bool success = true;
        // A material is composed of
        // material id for reference by cells, etc.
        // zaids
        // XLIB identifiers (N/P/PN/E/H/A/S/T/D)LIB=abx
        // CONDuction COND=#
        // GAS= (0/1)
        // XSTEP identifiers (E/H)STEP=n
        // Refractive index REFI=A
        // cauchy coefficients REFC = A B C D
        // selmeier coeeficients REFS = B1 C2 B2 C2 B3 C3
        Material m;

        m.zaid_index = m_material_zaids.size();
        for (auto itr = material_node.begin(); itr != material_node.end(); itr.next())
        {
            auto child = itr.get();
            const std::string& name = child.name();
            if (child.type() == mcnpi::NODE::LINE_CONTINUATION ||
                child.type() == mcnpi::NODE::COMMENT || name == "decl")
            {
                continue; // skip
            } 
            else if (name == "id")
            {
                m.id = std::stoi(child.data());
            }
            else if (name == "zaid")
            {
                success &= build_material_zaid(child, error);
            }
            // Materials declarations that have no impact on ZAID retrieval
            else if (name == "gas" || name == "estep" || name == "hstep" ||
                     name == "nlib" || name == "plib" || name == "pnlib" ||
                     name == "elib" || name == "hlib" || name == "alib" ||
                     name == "slib" || name == "tlib" || name == "dlib" ||
                     name == "cond" || name == "refi" || name == "refs" ||
                     name == "refc")
            {
               continue; // skip; not needed for materials construction
            }
            else
            {
                wasp_not_implemented(name << " is not supported");
            }
        }
        // Capture the count of the zaids constructed during this material's build
        m.zaid_count = m_material_zaids.size() - m.zaid_index;
        if (m.zaid_count == 0)
        {
            error << "*** Error -  line: " << material_node.line()
                  << ", column: " << material_node.column()
                  << " material has no ZAIDs specified!" << std::endl;
            return false;
        }
        if (m_material_id_index.find(m.id) != m_material_id_index.end())
        {
            error << "*** Error -  line: " << material_node.line()
                  << ", column: " << material_node.column()
                  << " material with id " << m.id << " already exists!" << std::endl;
            return false;
        }

        m_material_id_index.insert({m.id, m_materials.size()});
        m_materials.push_back(m);
        return success;
    }

    bool Model::build_material_zaid(const NodeView& zaid_node, std::ostream& error)
    {
        wasp_require(std::strcmp(zaid_node.name(),"zaid") == 0);
        Material_Zaid_Entry z;
        for (auto itr = zaid_node.begin(); itr != zaid_node.end(); itr.next())
        {
            auto child = itr.get();
            const std::string& name = child.name();
            auto data = child.data();
            if (name == "id")
            {
                z.zaid = std::stoi(data);
                // if the zaid's parse includes the lib numeric component, capture
                if (auto di = data.find('.') != std::string::npos)
                {
                    // captures decimal
                    auto sub = data.substr(di);
                    if (sub.size() > 1)
                    {
                        sub = sub.substr(1);
                        std::strcpy(z.abx,sub.data());
                    }
                }
            }
            else if (name == "value")
            {
                z.value = std::stod(data);
            }
            else if (name == "lib")
            {
                // wasp_check(data.size() < 4);
                std::strcpy(z.abx, data.data());
            }
            else if (name == "comment" || name == "LC")
            {
                std::cerr << "FOUND A COMMENT!" << std::endl;
                continue; // syntactically I don't think this will happen, but skip if it does
            }
            else
            {
                wasp_not_implemented("zaid member named "<< name);
            }
        }
                
        m_material_zaids.push_back(z);
        return true;
    }
    bool Model::build_sphere(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(),"sph") == 0);
        double origin[3] = {0,0,0};
        double r = 1;
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                if (name == "xbar")
                {
                    origin[0] = std::stod(child.data());
                }
                else if (name == "ybar")
                {
                    origin[1] = std::stod(child.data());
                }
                else if (name == "zbar")
                {
                    origin[2] = std::stod(child.data());
                }
                else if (name == "r")
                {
                    r = std::stod(child.data());
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = 4; // x,y,z,r
        surf.type = Surface_Type::SPH;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(origin), std::end(origin));
        m_surface_coeffs.push_back(r);
        return success;
    } // end of sphere

    bool Model::build_simple_quadric(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(),"sq") == 0);
        double coeffs[10] = {0,0,0,0,0,0,0,0,0,0};
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                if (name.size() == 1 && name.find_first_of("abcdefg") == 0)
                {
                   coeffs[name.front()-'a'] = std::stod(child.data());
                }
                else if (name == "xbar")
                {
                    coeffs[7] = std::stod(child.data());
                }
                else if (name == "ybar")
                {
                    coeffs[8] = std::stod(child.data());
                }
                else if (name == "zbar")
                {
                    coeffs[9] = std::stod(child.data());
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = 10; // a,b,c,d,e,f,g,xbar,ybar,zbar
        surf.type = Surface_Type::SQ;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    } // end of simple_quadric
    bool Model::build_general_quadric(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(),"gq") == 0);
        double coeffs[10] = {0,0,0,0,0,0,0,0,0,0};
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                if (name.size() == 1 && name.find_first_of("abcdefghjk") == 0)
                {
                   size_t offset = name.find_first_of("jk") == 0 ? 1 : 0;
                   coeffs[name.front()-offset-'a'] = std::stod(child.data());
                } 
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = 10; // a,b,c,d,e,f,g,h,j,k
        surf.type = Surface_Type::GQ;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_cylinder(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(), "xcyl") == 0 ||
                     std::strcmp(surface_node.name(), "ycyl") == 0 ||
                     std::strcmp(surface_node.name(), "zcyl") == 0);
        double coeffs[1] = {1};
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                if (name == "r")
                {
                    coeffs[0] = std::stod(child.data());
                } 
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error <<"Error - line: "<<child.line() <<", column: " << child.column() 
                        << " " << name <<" is an unexpected component!" << std::endl;
                    success &= false;
                }
            }catch (const std::exception& e)
            {
                error <<"Error - line: "<<child.line() <<", column: " << child.column() 
                    << " " << name <<" with value '" << child.data() 
                    <<"' failed to convert to a double - " << e.what() << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = 1;

        std::string surface_name = surface_node.name();        
        if (surface_name[0] == 'x') surf.type = Surface_Type::CX;
        else if (surface_name[0] == 'y') surf.type = Surface_Type::CY;
        else if (surface_name[0] == 'z') surf.type = Surface_Type::CZ;        
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_pcylinder(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(),"cyl") == 0);
        double coeffs[3] = {0,0,1};
        size_t surface_index = m_surfaces.size();
        bool success = true;
        // Parrallel Cylinder has (Y,Z | X,Z | X,Y) , R coefficients
        bool has_xbar = false;
        bool has_ybar = false;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                if (name == "r")
                {
                    coeffs[2] = std::stod(child.data());
                }
                else if (name == "xbar")
                {
                    coeffs[0] = std::stod(child.data());
                    has_xbar = true;
                }
                else if (name == "ybar")
                {
                    // ybar index changes if xbar is present (i.e., this is C/Z)
                    coeffs[has_xbar ? 1 : 0] = std::stod(child.data());
                    has_ybar = true;
                }
                else if (name == "zbar")
                {
                    coeffs[1] = std::stod(child.data());
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = 3;

        std::string surface_name = surface_node.name();        
        if (has_xbar && has_ybar) surf.type = Surface_Type::C_Z;
        else if (has_xbar) surf.type = Surface_Type::C_Y;
        else if (has_ybar) surf.type = Surface_Type::C_X;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_cone(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(),"cone") == 0);
        std::vector<double> coeffs; // vx,vy,vz, hx,hy,hz, r1, r2
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                // parser ensures these are in order
                if (name.size()==2 && name.find_first_of("vhr") == 0)
                {
                    coeffs.push_back(std::stod(child.data()));
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = coeffs.size();
        surf.type = Surface_Type::TRC; // truncated right-angle cone
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_xyzcone(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(), "xcone") == 0 ||
                     std::strcmp(surface_node.name(), "ycone") == 0 ||
                     std::strcmp(surface_node.name(), "zcone") == 0);
        double coeffs[3] = {0,0,0}; // a=x,y,z,t^2, and +/-1 
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                if (name == "t")
                {
                    coeffs[1] = std::stod(child.data());
                }
                else if (name == "xbar")
                {
                    coeffs[0] = std::stod(child.data());
                }
                else if (name == "ybar")
                {
                    coeffs[0] = std::stod(child.data());
                }
                else if (name == "zbar")
                {
                    coeffs[0] = std::stod(child.data());
                }
                else if (name == "sense")
                {
                    coeffs[2] = std::stod(child.data());
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = 3;

        std::string surface_name = surface_node.name();        
        if (surface_name.at(0) == 'z') surf.type = Surface_Type::KZ;
        else if (surface_name.at(0) == 'y') surf.type = Surface_Type::KY;
        else if (surface_name.at(0) == 'x') surf.type = Surface_Type::KX;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_pcone(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(), "xpcone") == 0 ||
                     std::strcmp(surface_node.name(), "ypcone") == 0 ||
                     std::strcmp(surface_node.name(), "zpcone") == 0);
        double coeffs[5] = {0,0,0,0,0}; // x,y,z,t^2, and +/-1 
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                if (name == "t")
                {
                    coeffs[3] = std::stod(child.data());
                }
                else if (name == "xbar")
                {
                    coeffs[0] = std::stod(child.data());
                }
                else if (name == "ybar")
                {
                    coeffs[1] = std::stod(child.data());
                }
                else if (name == "zbar")
                {
                    coeffs[2] = std::stod(child.data());
                }
                else if (name == "sense")
                {
                    coeffs[4] = std::stod(child.data());
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = 5;

        std::string surface_name = surface_node.name();        
        if (surface_name.at(0) == 'z') surf.type = Surface_Type::K_Z;
        else if (surface_name.at(0) == 'y') surf.type = Surface_Type::K_Y;
        else if (surface_name.at(0) == 'x') surf.type = Surface_Type::K_X;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }
    bool Model::build_torus(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(), "xtorus") == 0 ||
                     std::strcmp(surface_node.name(), "ytorus") == 0 ||
                     std::strcmp(surface_node.name(), "ztorus") == 0);
        double coeffs[6] = {0,0,0,0,0}; // x,y,zbar,a,b,c
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                if (name.size() == 1 && name.find_first_of("abc") == 0)
                {
                    coeffs[name.at(0)-'a'+3] = std::stod(child.data());
                }
                else if (name == "xbar")
                {
                    coeffs[0] = std::stod(child.data());
                }
                else if (name == "ybar")
                {
                    coeffs[1] = std::stod(child.data());
                }
                else if (name == "zbar")
                {
                    coeffs[2] = std::stod(child.data());
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = 6;

        std::string surface_name = surface_node.name();        
        if (surface_name.at(0) == 'z') surf.type = Surface_Type::TZ;
        else if (surface_name.at(0) == 'y') surf.type = Surface_Type::TY;
        else if (surface_name.at(0) == 'x') surf.type = Surface_Type::TX;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_plane(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(), "xpl") == 0 ||
                     std::strcmp(surface_node.name(), "ypl") == 0 ||
                     std::strcmp(surface_node.name(), "zpl") == 0);
        double coeffs[1] = {0}; // x,y,z intersect
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                if (name == "d")
                {
                    coeffs[0] = std::stod(child.data());
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else
                {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = 1;

        std::string surface_name = surface_node.name();        
        if (surface_name.at(0) == 'z') surf.type = Surface_Type::PZ;
        else if (surface_name.at(0) == 'y') surf.type = Surface_Type::PY;
        else if (surface_name.at(0) == 'x') surf.type = Surface_Type::PX;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_points(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(), "xpts") == 0 ||
                     std::strcmp(surface_node.name(), "ypts") == 0 ||
                     std::strcmp(surface_node.name(), "zpts") == 0);
        std::vector<double> coeffs; // (x,y,z), rp ... 
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                if (name == "xp" || name == "yp" || name == "zp" || name == "rp")
                {
                    coeffs.push_back(std::stod(child.data()));
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else
                {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = coeffs.size();

        std::string surface_name = surface_node.name();        
        if (surface_name.at(0) == 'z') surf.type = Surface_Type::Z;
        else if (surface_name.at(0) == 'y') surf.type = Surface_Type::Y;
        else if (surface_name.at(0) == 'x') surf.type = Surface_Type::X;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_abcplane(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(),"pl") == 0);
        std::vector<double> coeffs; // a,b,c,d
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                // parser ensures these are in order
                if (name == "a" || name == "b" || name == "c" || name == "d")
                {
                    coeffs.push_back(std::stod(child.data()));
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = coeffs.size();
        surf.type = Surface_Type::PLANE;        
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }
    bool Model::build_box(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(),"box") == 0);
        std::vector<double> coeffs; // vx,vy,vz, ax,ay,az, ax,ay,az, ax,ay,az
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end();
             itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                // parser ensures these are in order
                if (name == "vx" || name == "vy" || name == "vz" ||
                    name == "ax" || name == "ay" || name == "az")
                {
                    coeffs.push_back(std::stod(child.data()));
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = coeffs.size();
        surf.type = Surface_Type::BOX;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }
    bool Model::build_hexagon(const NodeView& surface_node, std::ostream& error)
    {
        wasp_require(std::strcmp(surface_node.name(),"hex") == 0);
        std::vector<double> coeffs; // vx,vy,vz, hx,hy,hz, rx,ry,rz, sx,sy,sz, tx,ty,tz
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try
            {
                // parser ensures these are in order
                if (name.size() == 2 && name.find_first_of("vhrst") == 0)
                {
                    coeffs.push_back(std::stod(child.data()));
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = coeffs.size();
        surf.type = Surface_Type::HEX;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_rectangular_parallelepiped(const NodeView& surface_node,
                                                 std::ostream&   error)
    {
        wasp_require(std::strcmp(surface_node.name(),"rppiped") == 0);
        std::vector<double> coeffs; // xmin/max, ymin/max, zmin/max
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end();
             itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                // parser ensures these are in order
                if (name.size()==4 && name.find_first_of("xyz") == 0)
                {
                    coeffs.push_back(std::stod(child.data()));
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = coeffs.size();
        surf.type = Surface_Type::RPP;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_right_rectangular_cylinder(const NodeView& surface_node,
                                                 std::ostream&   error)
    {
        wasp_require(std::strcmp(surface_node.name(),"rrectcyl") == 0);
        std::vector<double> coeffs; // vx,y,z and hx,y,z
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                // parser ensures these are in order
                if (name.size()<=2 && name.find_first_of("vhr") == 0)
                {
                    coeffs.push_back(std::stod(child.data()));
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else
                {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = coeffs.size();
        surf.type = Surface_Type::RCC;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_right_eliptical_cylinder(const NodeView& surface_node,
                                               std::ostream&   error)
    {
        wasp_require(std::strcmp(surface_node.name(),"relipcyl") == 0);
        std::vector<double> coeffs; // vx,y,z and hx,y,z
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                // parser ensures these are in order (vx, vx1, vy1... r)
                if (name.size()<=3 && name.find_first_of("vhr") == 0)
                {
                    coeffs.push_back(std::stod(child.data()));
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else
                {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = coeffs.size();
        surf.type = Surface_Type::REC;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    bool Model::build_plane_points(const NodeView& surface_node,
                                   std::ostream&   error)
    {

        wasp_require(std::strcmp(surface_node.name(),"plpts") == 0);
        std::vector<double> coeffs; // x,y,z x,y,z x,y,z (named xp,yp,zp)
        size_t surface_index = m_surfaces.size();
        bool success = true;
        for (auto itr = surface_node.begin(); itr != surface_node.end(); itr.next())
        {
            auto child = itr.get();
            std::string name = child.name();
            try{
                // parser ensures these are in order (x,y,z... )
                if (name.size()==2 && name.find_first_of("xyz") == 0)
                {
                    coeffs.push_back(std::stod(child.data()));
                }
                else if (name == "decl" || name == "comment")
                {
                    continue;
                }
                else
                {
                    error << "Error - line: " << child.line()
                          << ", column: " << child.column() << " " << name
                          << " is an unexpected component!" << std::endl;
                    success &= false;
                }
            }
            catch (const std::exception& e)
            {
                error << "Error - line: " << child.line()
                      << ", column: " << child.column() << " " << name
                      << " with value '" << child.data()
                      << "' failed to convert to a double - " << e.what()
                      << std::endl;
                success &= false;
            }
        }
        m_surfaces.resize(surface_index+1);
        auto& surf = m_surfaces[surface_index];
        surf.coeff_start = m_surface_coeffs.size();
        surf.coeff_count = coeffs.size();
        surf.type = Surface_Type::PLANE;
        m_surface_coeffs.insert(m_surface_coeffs.end(), std::begin(coeffs), std::end(coeffs));
        return success;
    }

    std::vector<double> Model::surface_coefficients(size_t surface_index) const 
    {
        wasp_check(surface_index < m_surfaces.size());
        size_t start = m_surfaces[surface_index].coeff_start;
        size_t end = m_surfaces[surface_index].coeff_start + m_surfaces[surface_index].coeff_count;
        return std::vector<double>(m_surface_coeffs.begin()+start, m_surface_coeffs.begin()+end);
    }

    Model::Surface_Type Model::surface_type(size_t surface_index) const
    {
        wasp_check(surface_index < m_surfaces.size());
        return m_surfaces[surface_index].type;
    }

    size_t Model::surface_index(int surface_id) const
    {
        wasp_check(surface_id > 0);
        static auto comp = [](const Surface& a, int sid) -> bool 
        {
            return a.id < sid;
        };
        auto itr = std::lower_bound(m_surfaces.begin(), m_surfaces.end(), surface_id, comp);
        if (itr != m_surfaces.end() && itr->id == surface_id)
            return std::distance(m_surfaces.begin(), itr);
        return m_surfaces.size();
    }

    size_t Model::cell_index(int cell_id) const
    {
        wasp_check(cell_id > 0);
        static auto comp = [](const Cell& c, int cid) -> bool 
        {
            return c.id < cid;
        };
        auto itr = std::lower_bound(m_cells.begin(), m_cells.end(), cell_id, comp);
        if (itr != m_cells.end() && itr->id == cell_id)
            return std::distance(m_cells.begin(), itr);
        return m_cells.size();
    }

    size_t Model::find_cell_index(int cell_id) const
    {
        wasp_check(cell_id > 0);
        auto comp = [cell_id](const Cell& c) -> bool 
        {
            return c.id == cell_id;
        };
        // reverse find as users typically use recently defined cells
        auto itr = std::find_if(m_cells.rbegin(), m_cells.rend(), comp);
        if (itr == m_cells.rend()) return m_cells.size();
        // reverse iterator distance gives distance from end to location, adjust to return
        // distance from beginning of array to found location
        return m_cells.size() - std::distance(m_cells.rbegin(), itr) - 1;
    }

    Model::CSG_OP Model::csg_node_operator(size_t csg_node_index) const
    {
        wasp_require(csg_node_index < m_cell_csg_nodes.size());
        return m_cell_csg_nodes[csg_node_index].op;
    }

    Model::Surface_Sense Model::csg_left_sense(size_t csg_node_index) const
    {
        wasp_require(csg_node_index < m_cell_csg_nodes.size());
        return m_cell_csg_nodes[csg_node_index].left_sense;
    }

    Model::Surface_Sense Model::csg_right_sense(size_t csg_node_index) const
    {
        wasp_require(csg_node_index < m_cell_csg_nodes.size());
        return m_cell_csg_nodes[csg_node_index].right_sense;
    }

    size_t Model::csg_node_left(size_t csg_node_index) const
    {
        wasp_require(csg_node_index < m_cell_csg_nodes.size());
        return m_cell_csg_nodes[csg_node_index].left;
    }

    size_t Model::csg_node_right(size_t csg_node_index) const
    {
        wasp_require(csg_node_index < m_cell_csg_nodes.size());
        return m_cell_csg_nodes[csg_node_index].right;
    }

    double Model::surface_coefficient(size_t coeff_index) const
    {
        wasp_require(coeff_index < m_surface_coeffs.size());
        return m_surface_coeffs[coeff_index];
    }
} // end of namespace mcnpi
} // end of namespace wasp
