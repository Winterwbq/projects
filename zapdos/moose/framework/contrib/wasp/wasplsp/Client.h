#ifndef WASPLSP_CLIENT_H
#define WASPLSP_CLIENT_H

#include <string>
#include <set>
#include <map>
#include <vector>
#include "waspcore/decl.h"
#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "wasplsp/SymbolIterator.h"
#include "waspcore/Object.h"

namespace wasp {
namespace lsp  {

template <class T>
class WASP_PUBLIC Client
{
  public:
    Client(){}

    ~Client(){}

    /** Enable client snippet capability in server initialize by toggling true
     * completionClientCapabilities/textDocument/completionItem/snippetSupport
     */
    void enableSnippetSupport()
    {
        return Impl.enableSnippetSupport();
    }

    /** Turn on client watch file registration and relative patterns with
     * capabilities/workspace/didChangeWatchedFiles/dynamicRegistration and
     * capabilities/workspace/didChangeWatchedFiles/relativePatternSupport
     */
    void enableWatcherSupport()
    {
        return Impl.enableWatcherSupport();
    }

    /** Check if watch file registration and relative patterns is enabled
     * capabilities/workspace/didChangeWatchedFiles/dynamicRegistration and
     * capabilities/workspace/didChangeWatchedFiles/relativePatternSupport
     * @return - true if this client expect watch files to get registered
     */
    bool hasWatcherSupport()
    {
        return Impl.hasWatcherSupport();
    }

    /** Enable client extension capability of given name in server initialize
     * @param method_name - sets capabilities/extensions/<method_name> = true
     */
    void enableExtension( const std::string & method_name )
    {
        return Impl.enableExtension( method_name );
    }

    /** Check if connected server supports provided extension method name
     * @param method_name - method name to check with server capabilities
     */
    bool serverSupportsExtension( const std::string & method_name )
    {
        return Impl.serverSupportsExtension( method_name );
    }

    /** set the client's connection shared_ptr to the provided connection
     * @return - true if not already connection and successfully set
     */
    bool connect( Connection::SP connection )
    {
        return Impl.connect( connection );
    }

    /** convenience method used to handle the initialize / initialized handshake
     * @return - true if initialize / initialized handshake handled successfully
     */
    bool startUpCleanly()
    {
        return Impl.startUpCleanly();
    }

    /** convenience method used to handle the document-close / shutdown / exit
     * @return - true if document-close / shutdown / exit handled successfully
     */
    bool wrapUpCleanly()
    {
        return Impl.wrapUpCleanly();
    }

    /** build initialize request object / write to connection / read response
     * @return - true if build / write / response read handled successfully
     */
    bool doInitialize()
    {
        return Impl.doInitialize();
    }

    /** build initialized notification object / write to connection / no read
     * @return - true if build / write handled successfully
     */
    bool doInitialized()
    {
        return Impl.doInitialized();
    }

    /** build didopen notification / write to connection / read diagnostics
     * @param document_path - document URI ( should be in proper URI format )
     * @param document_language_id - document language id string
     * @param document_text - full text of the document
     * @return - true if build / write / diagnostics read handled successfully
     */
    bool doDocumentOpen( const std::string & document_path        ,
                         const std::string & document_language_id ,
                         const std::string & document_text        )
    {
        return Impl.doDocumentOpen( document_path        ,
                                    document_language_id ,
                                    document_text        );
    }

    /** build didchange notification / write to connection / read diagnostics
     * @param start_line - starting line number of change ( zero-based )
     * @param start_character - starting column number of change ( zero-based )
     * @param end_line - ending line number of change ( zero-based )
     * @param end_character - ending column number of change ( zero-based )
     * @param range_length - range length of document change
     * @param new_document_text - text of the change over the provided range
     * @return - true if build / write / diagnostics read handled successfully
     */
    bool doDocumentChange( int                 start_line        ,
                           int                 start_character   ,
                           int                 end_line          ,
                           int                 end_character     ,
                           int                 range_length      ,
                           const std::string & new_document_text )
    {
        return Impl.doDocumentChange( start_line        ,
                                      start_character   ,
                                      end_line          ,
                                      end_character     ,
                                      range_length      ,
                                      new_document_text );
    }

    /** build completion request object / write to connection / read response
     * @param line - line number of completion request ( zero-based )
     * @param character - column number of completion request ( zero-based )
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentCompletion( int line      ,
                               int character )
    {
        return Impl.doDocumentCompletion( line      ,
                                          character );
    }

    /** build definition request object / write to connection / read response
     * @param line - line number of definition request ( zero-based )
     * @param character - column number of definition request ( zero-based )
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentDefinition( int line      ,
                               int character )
    {
        return Impl.doDocumentDefinition( line      ,
                                          character );
    }

    /** build hover request / write to connection / read and store response
     * @param line - zero-based input document line number for this request
     * @param character - zero-based input document column for this request
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentHover(int line, int character)
    {
        return Impl.doDocumentHover(line, character);
    }

    /** build references request object / write to connection / read response
     * @param line - line number of references request ( zero-based )
     * @param character - column number of references request ( zero-based )
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentReferences( int  line                ,
                               int  character           ,
                               bool include_declaration )
    {
        return Impl.doDocumentReferences( line                ,
                                          character           ,
                                          include_declaration );
    }

    /** build formatting request object / write to connection / read response
     * @param tab_size - value of the size of a tab in spaces for formatting
     * @param insert_spaces - flag indicating whether to use spaces for tabs
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentFormatting( int  tab_size      ,
                               bool insert_spaces )
    {
        return Impl.doDocumentFormatting( tab_size      ,
                                          insert_spaces );
    }

    /** build symbols request object / write to connection / read response
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentSymbols()
    {
        return Impl.doDocumentSymbols();
    }

    /** build didclose notification / write to connection / no read
     * @return - true if build / write handled successfully
     */
    bool doDocumentClose()
    {
        return Impl.doDocumentClose();
    }

    /** build shutdown request object / write to connection / read response
     * @return - true if build / write / response read handled successfully
     */
    bool doShutdown()
    {
        return Impl.doShutdown();
    }

    /** build exit notification object / write to connection / no read
     * @return - true if build / write handled successfully
     */
    bool doExit()
    {
        return Impl.doExit();
    }

    /** get the size of the response currently stored if it is DIAGNOSTICS
     * @return - size of diagnostics list if current response is expected type
     */
    int getDiagnosticSize()
    {
        return Impl.getDiagnosticSize();
    }

    /** get the size of the response currently stored if it is COMPLETION
     * @return - size of diagnostics list if current response is expected type
     */
    int getCompletionSize()
    {
        return Impl.getCompletionSize();
    }

    /** get the size of the response currently stored if it is DEFINITION
     * @return - size of diagnostics list if current response is expected type
     */
    int getDefinitionSize()
    {
        return Impl.getDefinitionSize();
    }

    /** get the size of the response currently stored if it is REFERENCES
     * @return - size of diagnostics list if current response is expected type
     */
    int getReferencesSize()
    {
        return Impl.getReferencesSize();
    }

    /** get the size of the response currently stored if it is FORMATTING
     * @return - size of diagnostics list if current response is expected type
     */
    int getFormattingSize()
    {
        return Impl.getFormattingSize();
    }

    /** get parameters at index into references if response type is DIAGNOSTICS
     * @param index - index of diagnostic object in response array to dissect
     * @param start_line - starting line number of diagnostic ( zero-based )
     * @param start_character - starting column number of diagnostic ( zero-based )
     * @param end_line - ending line number of diagnostic ( zero-based )
     * @param end_character - ending column number of diagnostic ( zero-based )
     * @param severity - diagnostic severity number
     * @param code - reference to diagnostic code string
     * @param source - reference to diagnostic source string
     * @param message - reference to diagnostic message string
     * @return - true if response type right / index valid / parameters are set
     */
    bool getDiagnosticAt( int                index      ,
                          clientDiagnostic & diagnostic )
    {
        return Impl.getDiagnosticAt( index      ,
                                     diagnostic );
    }

    /** get parameters at index into references if response type is COMPLETION
     * @param index - index of completion object in response array to dissect
     * @param label - label to be used for completion in autocomplete list
     * @param start_line - starting line for completion ( zero-based )
     * @param start_character - starting column for completion( zero-based )
     * @param end_line - ending line for completion ( zero-based )
     * @param end_character - ending column for completion ( zero-based )
     * @param new_text - text to be inserted for completion upon autocomplete
     * @param kind - kind value for completion
     * @param detail - detail string for completion
     * @param documentation - documentation string for completion
     * @param deprecated - flag indicating if completion is deprecated
     * @param preselect - flag indicating if completion should be pre-selected
     * @return - true if response type right / index valid / parameters are set
     */
    bool getCompletionAt( int                index      ,
                          clientCompletion & completion )
    {
        return Impl.getCompletionAt( index      ,
                                     completion );
    }

    /** get parameters at index into references if response type is DEFINITION
     * @param index - index of definition object in response array to dissect
     * @param start_line - starting line for definition ( zero-based )
     * @param start_character - starting column for definition ( zero-based )
     * @param end_line - ending line for definition ( zero-based )
     * @param end_character - ending column for definition ( zero-based )
     * @return - true if response type right / index valid / parameters are set
     */
    bool getDefinitionAt( int                index      ,
                          clientDefinition & definition )
    {
        return Impl.getDefinitionAt( index      ,
                                     definition );
    }

    /** get hover text string stored for response if HOVER is response type
     * @param hover_text - string reference to fill with text from response
     * @return - true if response type is HOVER / id matches / text was set
     */
    bool getHoverText(std::string & hover_text)
    {
        return Impl.getHoverText(hover_text);
    }

    /** get parameters at index into references if response type is REFERENCES
     * @param index - index of reference object in response array to dissect
     * @param start_line - starting line for reference ( zero-based )
     * @param start_character - starting column for reference ( zero-based )
     * @param end_line - ending line for reference ( zero-based )
     * @param end_character - ending column for reference ( zero-based )
     * @return - true if response type right / index valid / parameters are set
     */
    bool getReferencesAt( int                index     ,
                          clientReference & references )
    {
        return Impl.getReferencesAt( index      ,
                                     references );
    }

    /** get parameters at index into references if response type is FORMATTING
     * @param index - index of text edit object in response array to dissect
     * @param start_line - starting line for text edit ( zero-based )
     * @param start_character - starting column for text edit ( zero-based )
     * @param end_line - ending line for text edit ( zero-based )
     * @param end_character - ending column for text edit ( zero-based )
     * @param new_text - new text that belongs over the given range
     * @return - true if response type right / index valid / parameters are set
     */
    bool getFormattingAt( int                index      ,
                          clientFormatting & formatting )
    {
        return Impl.getFormattingAt( index      ,
                                     formatting );
    }

    /** build extension request / write to connection / get back response
     * @param line - zero-based input line number to use for this request
     * @param character - zero-based input column to use for this request
     * @return - true if build / write / response read handled successful
     * @return - true if successful request build / write / response read
     */
    bool doExtensionMethod( const std::string & extension_method ,
                            int                 line             ,
                            int                 character       )
    {
        return Impl.doExtensionMethod( extension_method ,
                                       line             ,
                                       character        );
    }

    /** get size of result currently stored if EXTENSION is response type
     * @return - size of result list if current response is expected type
     */
    int getExtensionResponseSize()
    {
        return Impl.getExtensionResponseSize();
    }

    /** get extension response stored at given index if EXTENSION is type
     * @param extension_response - object to fill with extension response
     * @return - true if response type is EXTENSION and index is in range
     */
    bool getExtensionResponseAt( int                index             ,
                                 wasp::DataObject & extension_response )
    {
        return Impl.getExtensionResponseAt( index              ,
                                            extension_response );
    }

    /** handle unregister and register requests by server for watch files
     ** this is called by LSPInterpreter if server has capability enabled
     * @return - true if unregister and register requests were successful
     */
    bool handleWatchFileRegistration()
    {
        return Impl.handleWatchFileRegistration();
    }

    /** get set of all current files that client should watch for changes
     ** this can be called by Workbench to get list of all files to watch
     * @return - set of files that client should tell server when changed
     */
    std::set<std::string> getAllWatchFiles() const
    {
        return Impl.getAllWatchFiles();
    }

    /** notify server about any changed resources and get all diagnostics
     ** this can be called by Workbench when any of it watch files change
     * @param resource_uris - set of uris for resources that have changed
     * @param all_diagnostics - map of file uris to clientDiagnostic list
     * @return - true if notification and diagnostics read was successful
     */
    bool notifyServerChangedWatchedFiles(
        const std::set<std::string>                          & resource_uris   ,
        std::map<std::string, std::vector<clientDiagnostic>> & all_diagnostics )
    {
        return Impl.notifyServerChangedWatchedFiles( resource_uris   ,
                                                     all_diagnostics );
    }

    /** check if the client is properly connected for reading / writing
     * @return - true if the client is properly connected for reading / writing
     */
    bool isConnected()
    {
        return Impl.isConnected();
    }

    /** get a shared pointer to this client's read / write connection
     * @return - shared pointer to the client's read / write connection
     */
    Connection::SP getConnection()
    {
        return Impl.getConnection();
    }

    /** get all errors that have been stored on the client for any reason
     * @return - error string of all errors that have been stored on the client
     */
    std::string getErrors() const
    {
        return Impl.getErrors();
    }

    /** get previous request id used by the client for a request to the server
     * @return - integer id of the previous request to the server
     */
    int getPreviousRequestID() const
    {
        return Impl.getPreviousRequestID();
    }

    /** check if the client currently has a document open
     * @return - true if client currently has a document open
     */
    bool isDocumentOpen() const
    {
        return Impl.isDocumentOpen();
    }

    /** get the document path of the currently open document on the client
     * @return - document path string of the open document on the client
     */
    std::string getDocumentPath() const
    {
        return Impl.getDocumentPath();
    }

    /** get current document version number which should increase on each change
     * @return - integer id of the current document version number
     */
    int getCurrentDocumentVersion() const
    {
        return Impl.getCurrentDocumentVersion();
    }

    /** get a shared pointer to a symbol iterator for the client symbol response
    * @return - shared pointer to a symbol iterator
     */
    SymbolIterator::SP getSymbolIterator()
    {
        return Impl.getSymbolIterator();
    }

  private:

    /**
     * @brief Impl - client implementation that each of the above methods use
     */
    T Impl;
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_CLIENT_H
