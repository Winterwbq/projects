#include "ServerImpl.h"

namespace wasp {
namespace lsp  {

bool ServerImpl::run()
{
    bool pass = true;

    // loop reading from connection and handling methods until error or exit

    while(1)
    {
        DataObject  input_object;
        std::string method_name;
        DataObject  output_object;
        DataArray   output_array;

        // read from the connection into the input_object

        pass &= this->connectionRead( input_object );

        // get the method name from the input_object if it exists

        if ( input_object.contains(m_method) && input_object[m_method].is_string() )
        {
            method_name = input_object[m_method].to_string();
        }

        // call the specific handler on the input_object for the method read

        if ( method_name == m_method_initialize )
        {
            pass &= this->handleInitializeRequest( input_object  ,
                                                   output_object );
        }
        else if ( method_name == m_method_initialized )
        {
            pass &= this->handleInitializedNotification( input_object );
        }
        else if ( method_name == m_method_didopen )
        {
            pass &= this->handleDidOpenNotification( input_object  ,
                                                     output_object );
        }
        else if ( method_name == m_method_didchange)
        {
            pass &= this->handleDidChangeNotification( input_object  ,
                                                       output_object );
        }
        else if ( method_name == m_method_didchangewatch )
        {
            pass &= this->handleDidChangeWatchedFilesNotification( input_object ,
                                                                   output_array );
        }
        else if ( method_name == m_method_completion )
        {
            pass &= this->handleCompletionRequest( input_object  ,
                                                   output_object );
        }
        else if ( method_name == m_method_definition )
        {
            pass &= this->handleDefinitionRequest( input_object  ,
                                                   output_object );
        }
        else if ( method_name == m_method_hover )
        {
            pass &= this->handleHoverRequest( input_object  ,
                                              output_object );
        }
        else if ( method_name == m_method_references )
        {
            pass &= this->handleReferencesRequest( input_object  ,
                                                   output_object );
        }
        else if ( method_name == m_method_formatting )
        {
            pass &= this->handleFormattingRequest( input_object  ,
                                                   output_object );
        }
        else if ( method_name == m_method_documentsymbol )
        {
            pass &= this->handleSymbolsRequest( input_object  ,
                                                output_object );
        }
        else if ( method_name == m_method_didclose )
        {
            pass &= this->handleDidCloseNotification( input_object );
        }
        else if ( method_name == m_method_shutdown )
        {
            pass &= this->handleShutdownRequest( input_object  ,
                                                 output_object );
        }
        else if ( method_name == m_method_exit )
        {
            pass &= this->handleExitNotification( input_object );
        }

        // if request method is unknown then send back error and keep going
        // unless method is supported by client then call extension handler
        else if ( objectHasRequestId(input_object) && !method_name.empty() )
        {
            if ( !this->clientSupportsExtension(method_name) )
            {
                this->errors << m_error_prefix
                             << "Server request has unsupported method: "
                             << "\"" << method_name << "\"" << std::endl;
                buildErrorResponse( output_object            ,
                                    m_method_not_found_error ,
                                    this->errors.str()       );
            }
            else pass &= this->handleExtensionRequest( method_name   ,
                                                       input_object  ,
                                                       output_object );
        }

        // if anything failed in the process, then build an error response
        if ( !pass )
        {
            buildErrorResponse( output_object      ,
                                m_internal_error   ,
                                this->errors.str() );

            this->is_initialized = false;
        }

        // write either response object or all items in array to connection
        if ( !output_object.empty() )
        {
            pass &= this->connectionWrite( output_object );
        }
        else if ( !output_array.empty() )
        {
            for (std::size_t i = 0; i < output_array.size(); i++)
            {
                pass &= this->connectionWrite(*(output_array.at(i).to_object()));
            }
        }

        // send client list of document resources if symbols were just sent
        // if client supports watch file registration and relative patterns
        if (method_name == m_method_documentsymbol && this->client_watcher_support)
        {
            pass &= this->registerWatchFiles();
        }

        // if anything failed or if the method was 'exit' - then stop reading

        if ( !pass || method_name == m_method_exit ) break;
    }

    return pass;
}

bool ServerImpl::handleInitializeRequest(
                const DataObject & initializeRequest  ,
                      DataObject & initializeResponse )
{
    if ( this->is_initialized )
    {
        this->errors << m_error_prefix << "Server already initialized" << std::endl;
        return false;
    }

    bool pass = true;

    // dissect initialize request object

    pass &= dissectInitializeRequest( initializeRequest         ,
                                      this->errors              ,
                                      this->client_request_id   ,
                                      this->client_process_id   ,
                                      this->client_root_path    ,
                                      this->client_capabilities );

    // check if client announces snippet completion support in request at
    // capabilities/textDocument/completion/completionItem/snippetSupport
    if (client_capabilities.contains(m_text_document) &&
        client_capabilities[m_text_document].is_object())
    {
        const auto & textdoc_caps = *(client_capabilities[m_text_document].to_object());
        if (textdoc_caps.contains(m_comp) && textdoc_caps[m_comp].is_object())
        {
            const auto & comp_caps = *(textdoc_caps[m_comp].to_object());
            if (comp_caps.contains(m_compitem) && comp_caps[m_compitem].is_object())
            {
                const auto & compitem_caps = *(comp_caps[m_compitem].to_object());
                if (compitem_caps.contains(m_snip) && compitem_caps[m_snip].is_bool())
                {
                    this->client_snippet_support = compitem_caps[m_snip].to_bool();
                }
            }
        }
    }

    // check if client allows watch file registration and relative patterns
    // capabilities/workspace/didChangeWatchedFiles/dynamicRegistration and
    // capabilities/workspace/didChangeWatchedFiles/relativePatternSupport
    if (client_capabilities.contains(m_workspace) &&
        client_capabilities[m_workspace].is_object())
    {
        const auto & workspace_caps = *(client_capabilities[m_workspace].to_object());
        if (workspace_caps.contains(m_change_watched_files) &&
            workspace_caps[m_change_watched_files].is_object())
        {
            const auto & watch_caps = *(workspace_caps[m_change_watched_files].to_object());
            if (watch_caps.contains(m_dynamic_registration) &&
                watch_caps[m_dynamic_registration].is_bool())
            {
                if (watch_caps[m_dynamic_registration].to_bool())
                {
                    if (watch_caps.contains(m_relative_patterns) &&
                        watch_caps[m_relative_patterns].is_bool())
                    {
                        if (watch_caps[m_relative_patterns].to_bool())
                        {
                            this->client_watcher_support = true;
                        }
                    }
                }
            }
        }
    }

    // cache extension method names announced in client capabilities from
    // capabilities/extensions/<method_name> = true
    if (client_capabilities.contains(m_extensions) &&
        client_capabilities[m_extensions].is_object())
        for (const auto & c : *(client_capabilities[m_extensions].to_object()))
            if (c.second.is_bool() && c.second.to_bool())
                this->client_extension_methods.insert(c.first);

    // build initialize response object

    pass &= buildInitializeResponse( initializeResponse        ,
                                     this->errors              ,
                                     this->client_request_id   ,
                                     this->server_capabilities );

    if ( pass ) this->is_initialized = true;

    return pass;
}

bool ServerImpl::handleInitializedNotification(
                const DataObject & initializedNotification )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    // dissect initialized notification object - no response is needed

    pass &= dissectInitializedNotification( initializedNotification ,
                                            this->errors            );

    return pass;
}

bool ServerImpl::handleDidOpenNotification(
                const DataObject & didOpenNotification            ,
                      DataObject & publishDiagnosticsNotification )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    std::string document_path;
    int         document_version;

    // dissect didopen notification object

    pass &= dissectDidOpenNotification( didOpenNotification        ,
                                        this->errors               ,
                                        document_path              ,
                                        this->document_language_id ,
                                        document_version           ,
                                        this->document_text        );

    if (this->document_versions.count(document_path))
    {
        this->errors << m_error_prefix << "Server already opened '" << document_path << "'" << std::endl;
        return false;
    }

    // set document version in map for current input path to provided value
    this->document_versions[document_path] = document_version;

    // set current document path on server to input path for this operation
    this->document_path = document_path;

    // put initial document content in map from file paths to current input
    this->path_to_text[this->document_path] = this->document_text;

    DataArray document_diagnostics;

    // call server specific method to parse the document and gather diagnostics

    pass &= this->parseDocumentForDiagnostics( document_diagnostics );

    // build diagnostics notification object

    pass &= buildPublishDiagnosticsNotification( publishDiagnosticsNotification ,
                                                 this->errors                   ,
                                                 this->document_path            ,
                                                 document_diagnostics           );

    return pass;
}

bool ServerImpl::handleDidChangeNotification(
                const DataObject & didChangeNotification          ,
                      DataObject & publishDiagnosticsNotification )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    std::string document_path;
    int         document_version;
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
    int         range_length;
    std::string replacement_text;

    // dissect didchange notification object

    pass &= dissectDidChangeNotification( didChangeNotification  ,
                                          this->errors           ,
                                          document_path          ,
                                          document_version       ,
                                          start_line             ,
                                          start_character        ,
                                          end_line               ,
                                          end_character          ,
                                          range_length           ,
                                          replacement_text       );

    if (!this->document_versions.count(document_path))
    {
        this->errors << m_error_prefix << "Server has not opened '" << document_path << "'" << std::endl;
        return false;
    }

    if (document_version <= this->document_versions.find(document_path)->second)
    {
        this->errors << m_error_prefix << "Server got bad id for '" << document_path << "'" << std::endl;
        return false;
    }

    // set document version in map for current input path to provided value
    this->document_versions[document_path] = document_version;

    // set current document path on server to input path for this operation
    this->document_path = document_path;

    // call server specific method to update in-memory document with changes

    pass &= this->updateDocumentTextChanges( replacement_text ,
                                             start_line       ,
                                             start_character  ,
                                             end_line         ,
                                             end_character    ,
                                             range_length     );

    // update map from document paths to current input with changed content
    this->path_to_text[this->document_path] = this->document_text;

    DataArray document_diagnostics;

    // call server specific method to parse the document and gather diagnostics

    pass &= this->parseDocumentForDiagnostics( document_diagnostics );

    // build diagnostics notification object

    pass &= buildPublishDiagnosticsNotification( publishDiagnosticsNotification ,
                                                 this->errors                   ,
                                                 this->document_path            ,
                                                 document_diagnostics           );

    return pass;
}

bool ServerImpl::handleDidChangeWatchedFilesNotification(
                const DataObject & didChangeWatchedFilesNotification ,
                      DataArray  & publishDiagnosticsArray           )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    // dissect didChangeWatchedFiles notification object client sent server
    DataArray changes;
    pass &= dissectDidChangeWatchedFilesNotification( didChangeWatchedFilesNotification ,
                                                      this->errors                      ,
                                                      changes                           );

    // walk over changed resources array to gather all dependent base files
    std::set<std::string> all_base_uris;
    for (std::size_t i = 0; i < changes.size(); i++)
    {
        // dissect each change object in array for change file uri and type
        std::string uri;
        int         type;
        pass &= dissectChangeObject( *(changes.at(i).to_object()) ,
                                     this->errors                 ,
                                     uri                          ,
                                     type                         );

        // accumulate base files with dependencies on any changed resources
        if (type == m_change_type_changed)
        {
            const auto & dependent_base_uris = getBasesForResource(uri);
            all_base_uris.insert(dependent_base_uris.begin(), dependent_base_uris.end());
        }
    }

    // reprocess all base files that have dependencies on changed resources
    for (const auto & base_uri : all_base_uris)
    {
        // set current path and text for each base file before reprocessing
        this->document_path = base_uri;
        this->document_text = getDocumentText();

        // reprocess dependent base file to obtain array of its diagnostics
        DataArray document_diagnostics;
        pass &= parseDocumentForDiagnostics(document_diagnostics);

        // add each publishDiagnostics object into publishDiagnostics array
        publishDiagnosticsArray.push_back(wasp::DataObject());
        wasp::DataObject * publishDiagnosticsObject = publishDiagnosticsArray.back().to_object();
        pass &= buildPublishDiagnosticsNotification(*publishDiagnosticsObject ,
                                                    this->errors              ,
                                                    this->document_path       ,
                                                    document_diagnostics      );
    }

    // append sentinel object to tell client all diagnostics have been sent
    publishDiagnosticsArray.push_back(wasp::DataObject());
    wasp::DataObject * lastObject = publishDiagnosticsArray.back().to_object();
    pass &= buildDiagnosticsSentinelObject(*lastObject, this->errors);

    return pass;
}

bool ServerImpl::handleCompletionRequest(
                const DataObject & completionRequest  ,
                      DataObject & completionResponse )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    std::string document_path;
    int         line;
    int         character;

    // dissect completion request object

    pass &= dissectCompletionRequest( completionRequest       ,
                                      this->errors            ,
                                      this->client_request_id ,
                                      document_path           ,
                                      line                    ,
                                      character               );

    if (!this->document_versions.count(document_path))
    {
        this->errors << m_error_prefix << "Server has not opened '" << document_path << "'" << std::endl;
        return false;
    }

    // set current document path on server to input path for this operation
    this->document_path = document_path;

    DataArray completion_items;

    // indicate list is complete by default in case server does not specify
    bool is_incomplete = false;

    // call server specific method to gather the document completions items

    pass &= this->gatherDocumentCompletionItems( completion_items,
                                                 is_incomplete   ,
                                                 line            ,
                                                 character       );

    // build completion response object

    pass &= buildCompletionResponse( completionResponse      ,
                                     this->errors            ,
                                     this->client_request_id ,
                                     is_incomplete           ,
                                     completion_items        );

    return pass;
}

bool ServerImpl::handleDefinitionRequest(
                const DataObject & definitionRequest  ,
                      DataObject & definitionResponse )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    std::string document_path;
    int         line;
    int         character;

    // dissect definition request object

    pass &= dissectDefinitionRequest( definitionRequest       ,
                                      this->errors            ,
                                      this->client_request_id ,
                                      document_path           ,
                                      line                    ,
                                      character               );

    if (!this->document_versions.count(document_path))
    {
        this->errors << m_error_prefix << "Server has not opened '" << document_path << "'" << std::endl;
        return false;
    }

    // set current document path on server to input path for this operation
    this->document_path = document_path;

    DataArray definition_locations;

    // call server specific method to gather the document definition locations

    pass &= this->gatherDocumentDefinitionLocations( definition_locations ,
                                                     line                 ,
                                                     character            );

    // build definition response object

    pass &= buildLocationsResponse( definitionResponse      ,
                                    this->errors            ,
                                    this->client_request_id ,
                                    definition_locations    );

    return pass;
}

bool ServerImpl::handleHoverRequest(
                const DataObject & hoverRequest  ,
                      DataObject & hoverResponse )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    std::string document_path;
    int         line;
    int         character;

    // dissect hover request object
    pass &= dissectHoverRequest( hoverRequest            ,
                                 this->errors            ,
                                 this->client_request_id ,
                                 document_path           ,
                                 line                    ,
                                 character               );

    if (!this->document_versions.count(document_path))
    {
        this->errors << m_error_prefix << "Server has not opened '" << document_path << "'" << std::endl;
        return false;
    }

    // set current document path on server to input path for this operation
    this->document_path = document_path;

    // call server specific method to get hover display at request location
    std::string display_text;
    pass &= this->getHoverDisplayText(display_text, line, character);

    // build hover response object with hover text that should be displayed
    pass &= buildHoverResponse( hoverResponse           ,
                                this->errors            ,
                                this->client_request_id ,
                                display_text            );

    return pass;
}

bool ServerImpl::handleReferencesRequest(
                const DataObject & referencesRequest  ,
                      DataObject & referencesResponse )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    std::string document_path;
    int         line;
    int         character;
    bool        include_declaration;

    // dissect references request object

    pass &= dissectReferencesRequest( referencesRequest       ,
                                      this->errors            ,
                                      this->client_request_id ,
                                      document_path           ,
                                      line                    ,
                                      character               ,
                                      include_declaration     );

    if (!this->document_versions.count(document_path))
    {
        this->errors << m_error_prefix << "Server has not opened '" << document_path << "'" << std::endl;
        return false;
    }

    // set current document path on server to input path for this operation
    this->document_path = document_path;

    DataArray references_locations;

    // call server specific method to gather the document references locations

    pass &= this->gatherDocumentReferencesLocations( references_locations ,
                                                     line                 ,
                                                     character            ,
                                                     include_declaration  );

    // build references response object

    pass &= buildLocationsResponse( referencesResponse      ,
                                    this->errors            ,
                                    this->client_request_id ,
                                    references_locations    );

    return pass;
}

bool ServerImpl::handleFormattingRequest(
                const DataObject & formattingRequest  ,
                      DataObject & formattingResponse )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    std::string document_path;
    int         tab_size;
    bool        insert_spaces;

    // dissect formatting request object

    pass &= dissectFormattingRequest( formattingRequest       ,
                                      this->errors            ,
                                      this->client_request_id ,
                                      document_path           ,
                                      tab_size                ,
                                      insert_spaces           );

    if (!this->document_versions.count(document_path))
    {
        this->errors << m_error_prefix << "Server has not opened '" << document_path << "'" << std::endl;
        return false;
    }

    // set current document path on server to input path for this operation
    this->document_path = document_path;

    DataArray formatting_textedits;

    // call server specific method to gather the document formatting edits

    pass &= this->gatherDocumentFormattingTextEdits( formatting_textedits ,
                                                     tab_size             ,
                                                     insert_spaces        );

    // build formatting response object

    pass &= buildFormattingResponse( formattingResponse      ,
                                     this->errors            ,
                                     this->client_request_id ,
                                     formatting_textedits    );

    return pass;
}

bool ServerImpl::handleSymbolsRequest(
                const DataObject & symbolsRequest  ,
                      DataObject & symbolsResponse )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    std::string document_path;

    // dissect symbol request object

    pass &= dissectSymbolsRequest( symbolsRequest          ,
                                   this->errors            ,
                                   this->client_request_id ,
                                   document_path           );

    if (!this->document_versions.count(document_path))
    {
        this->errors << m_error_prefix << "Server has not opened '" << document_path << "'" << std::endl;
        return false;
    }

    // set current document path on server to input path for this operation
    this->document_path = document_path;

    DataArray document_symbols;

    // call server specific method to gather the document symbols

    pass &= this->gatherDocumentSymbols( document_symbols );

    // build symbol response object

    pass &= buildSymbolsResponse( symbolsResponse         ,
                                  this->errors            ,
                                  this->client_request_id ,
                                  document_symbols        );

    return pass;
}

bool ServerImpl::registerWatchFiles()
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    // build request to unregister client didChangeWatchedFiles of document
    DataObject unregister_request;
    pass &= buildUnregisterWatchFilesRequest(unregister_request,
                                             this->errors,
                                             this->server_request_id++,
                                             this->document_path);

    // request client to unregister document watch files before registering
    pass &= this->connectionWrite(unregister_request);

    // read response by client and make sure it indicates success not error
    DataObject unregister_response;
    pass &= this->connectionRead(unregister_response);
    pass &= checkErrorResponse(unregister_response, this->errors);

    // build request to register client didChangeWatchedFiles for resources
    const auto & resource_uris = getResourcesForBase(this->document_path);
    DataObject register_request;
    pass &= buildRegisterWatchFilesRequest(register_request,
                                           this->errors,
                                           this->server_request_id++,
                                           this->document_path,
                                           resource_uris);

    // request client to register document resources as files it will watch
    pass &= this->connectionWrite(register_request);

    // read response by client and make sure it indicates success not error
    DataObject register_response;
    pass &= this->connectionRead(register_response);
    pass &= checkErrorResponse(register_response, this->errors);

    return pass;
}

void ServerImpl::setResourcesForBase(const std::string           & base_uri,
                                     const std::set<std::string> & resource_uris)
{
    // remove base file from resource dependency map where previously added
    const auto it = base_to_resources_map.find(base_uri);
    if (it != base_to_resources_map.end())
    {
        for (const auto & resource : it->second)
        {
            auto & base_uris = resource_to_bases_map[resource];
            base_uris.erase(base_uri);
            if (base_uris.empty())
                resource_to_bases_map.erase(resource);
        }
    }

    // update resources for base file by overwriting any that already exist
    if (resource_uris.empty()) base_to_resources_map.erase(base_uri);
    else base_to_resources_map[base_uri] = resource_uris;

    // add base file into set of dependencies for every associated resource
    for (const auto & resource_uri : resource_uris)
        resource_to_bases_map[resource_uri].insert(base_uri);
}

std::set<std::string> ServerImpl::getBasesForResource(const std::string & resource_uri) const
{
    // return uris for all base files that set given resource as dependency
    const auto it = resource_to_bases_map.find(resource_uri);
    if (it != resource_to_bases_map.end())
        return it->second;
    return {};
}

std::set<std::string> ServerImpl::getResourcesForBase(const std::string & base_uri) const
{
    // return uris for all resource files set as dependencies of given base
    const auto it = base_to_resources_map.find(base_uri);
    if (it != base_to_resources_map.end())
        return it->second;
    return {};
}

bool ServerImpl::handleExtensionRequest(
                const std::string      & extensionMethod   ,
                const wasp::DataObject & extensionRequest  ,
                      wasp::DataObject & extensionResponse )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    std::string document_path;
    int         line;
    int         character;

    // dissect extension request object
    pass &= dissectExtensionRequest( extensionRequest        ,
                                     this->errors            ,
                                     this->client_request_id ,
                                     document_path           ,
                                     line                    ,
                                     character               );

    if (!this->document_versions.count(document_path))
    {
        this->errors << m_error_prefix << "Server has not opened '" << document_path << "'" << std::endl;
        return false;
    }

    // set current document path on server to input path for this operation
    this->document_path = document_path;

    DataArray extension_responses;

    // call server specific method to gather the extension responses
    pass &= this->gatherExtensionResponses( extension_responses ,
                                            extensionMethod     ,
                                            line                ,
                                            character           );

    // build extension response object
    pass &= buildExtensionResponse( extensionResponse       ,
                                    this->errors            ,
                                    this->client_request_id ,
                                    extension_responses     );

    return pass;
}

bool ServerImpl::handleDidCloseNotification(
                const DataObject & didCloseNotification )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    std::string document_path;

    // dissect didclose notification object - no response is needed

    pass &= dissectDidCloseNotification( didCloseNotification ,
                                         this->errors         ,
                                         document_path        );

    if (!this->document_versions.count(document_path))
    {
        this->errors << m_error_prefix << "Server has not opened '" << document_path << "'" << std::endl;
        return false;
    }

    // remove input path from map of open documents as it is no longer open
    this->document_versions.erase(document_path);

    return pass;
}

bool ServerImpl::handleShutdownRequest(
                const DataObject & shutdownRequest  ,
                      DataObject & shutdownResponse )
{
    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    // dissect shutdown request object

    pass &= dissectShutdownRequest( shutdownRequest         ,
                                    this->errors            ,
                                    this->client_request_id );

    this->is_initialized = false;

    // build shutdown response object

    pass &= buildShutdownResponse( shutdownResponse        ,
                                   this->errors            ,
                                   this->client_request_id );

    return pass;
}

bool ServerImpl::handleExitNotification(
                const DataObject & exitNotification )
{
    if (this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be shutdown" << std::endl;
        return false;
    }

    bool pass = true;

    // dissect exit notification object - no response is needed

    pass &= dissectExitNotification( exitNotification ,
                                     this->errors     );

    return pass;
}

} // namespace lsp
} // namespace wasp
