#ifndef WASPLSP_CLIENT_IMPL_H
#define WASPLSP_CLIENT_IMPL_H

#include <string>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "wasplsp/SymbolIterator.h"
#include "waspcore/Object.h"
#include "waspcore/decl.h"
#include "waspcore/wasp_bug.h"

namespace wasp {
namespace lsp  {

class WASP_PUBLIC ClientImpl
{
  public:

    ClientImpl() :
        is_connected(false)      ,
        is_initialized(false)    ,
        is_document_open(false)  ,
        already_in_call(false)   ,
        request_id(0)            ,
        document_version(0)      ,
        support_snippets(false)  ,
        support_watchers(false)  ,
        response_type(NONE){}

    ~ClientImpl(){}

    typedef std::shared_ptr<wasp::lsp::ClientImpl> SP;

    /** Enable client snippet capability in server initialize by toggling true
     * completionClientCapabilities/textDocument/completionItem/snippetSupport
     */
    void enableSnippetSupport();

    /** Turn on client watch file registration and relative patterns with
     * capabilities/workspace/didChangeWatchedFiles/dynamicRegistration and
     * capabilities/workspace/didChangeWatchedFiles/relativePatternSupport
     */
    void enableWatcherSupport();

    /** Check if watch file registration and relative patterns is enabled
     * capabilities/workspace/didChangeWatchedFiles/dynamicRegistration and
     * capabilities/workspace/didChangeWatchedFiles/relativePatternSupport
     * @return - true if this client expect watch files to get registered
     */
    bool hasWatcherSupport();

    /** Enable client extension capability of given name in server initialize
     * @param method_name - sets capabilities/extensions/<method_name> = true
     */
    void enableExtension(const std::string & method_name);

    /** Check if connected server supports provided extension method name
     * @param method_name - method name to check with server capabilities
     */
    bool serverSupportsExtension(const std::string & method_name);

    /** set the client's connection shared_ptr to the provided connection
     * @return - true if not already connection and successfully set
     */
    bool connect( wasp::lsp::Connection::SP connection );

    /** convenience method used to handle the initialize / initialized handshake
     * @return - true if initialize / initialized handshake handled successfully
     */
    bool startUpCleanly();

    /** convenience method used to handle the document-close / shutdown / exit
     * @return - true if document-close / shutdown / exit handled successfully
     */
    bool wrapUpCleanly();

    /** build initialize request object / write to connection / read response
     * @return - true if build / write / response read handled successfully
     */
    bool doInitialize();

    /** build initialized notification object / write to connection / no read
     * @return - true if build / write handled successfully
     */
    bool doInitialized();

    /** build didopen notification / write to connection / read diagnostics
     * @param document_path - document URI ( should be in proper URI format )
     * @param document_language_id - document language id string
     * @param document_text - full text of the document
     * @return - true if build / write / diagnostics read handled successfully
     */
    bool doDocumentOpen( const std::string & document_path        ,
                         const std::string & document_language_id ,
                         const std::string & document_text        );

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
                           const std::string & new_document_text );

    /** build completion request object / write to connection / read response
     * @param line - line number of completion request ( zero-based )
     * @param character - column number of completion request ( zero-based )
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentCompletion( int line      ,
                               int character );

    /** build definition request object / write to connection / read response
     * @param line - line number of definition request ( zero-based )
     * @param character - column number of definition request ( zero-based )
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentDefinition( int line      ,
                               int character );

    /** build hover request / write to connection / read and store response
     * @param line - zero-based input document line number for this request
     * @param character - zero-based input document column for this request
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentHover(int line, int character);

    /** build references request object / write to connection / read response
     * @param line - line number of references request ( zero-based )
     * @param character - column number of references request ( zero-based )
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentReferences( int  line                ,
                               int  character           ,
                               bool include_declaration );

    /** build formatting request object / write to connection / read response
     * @param tab_size - value of the size of a tab in spaces for formatting
     * @param insert_spaces - flag indicating whether to use spaces for tabs
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentFormatting( int  tab_size      ,
                               bool insert_spaces );

    /** build symbols request object / write to connection / read response
     * @return - true if build / write / response read handled successfully
     */
    bool doDocumentSymbols();

    /** build didclose notification / write to connection / no read
     * @return - true if build / write handled successfully
     */
    bool doDocumentClose();

    /** build shutdown request object / write to connection / read response
     * @return - true if build / write / response read handled successfully
     */
    bool doShutdown();

    /** build exit notification object / write to connection / no read
     * @return - true if build / write handled successfully
     */
    bool doExit();

    /** get the size of the response currently stored if it is DIAGNOSTICS
     * @return - size of diagnostics list if current response is expected type
     */
    int getDiagnosticSize();

    /** get the size of the response currently stored if it is COMPLETION
     * @return - size of diagnostics list if current response is expected type
     */
    int getCompletionSize();

    /** get the size of the response currently stored if it is DEFINITION
     * @return - size of diagnostics list if current response is expected type
     */
    int getDefinitionSize();

    /** get the size of the response currently stored if it is REFERENCES
     * @return - size of diagnostics list if current response is expected type
     */
    int getReferencesSize();

    /** get the size of the response currently stored if it is FORMATTING
     * @return - size of diagnostics list if current response is expected type
     */
    int getFormattingSize();

    /** get parameters at index into references if response type is DIAGNOSTICS
     * @param index - index of diagnostic object in response array to obtain
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
    bool getDiagnosticAt( int                           index      ,
                          wasp::lsp::clientDiagnostic & diagnostic );

    /** get parameters at index into references if response type is COMPLETION
     * @param index - index of completion object in response array to obtain
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
    bool getCompletionAt( int                           index      ,
                          wasp::lsp::clientCompletion & completion );

    /** get parameters at index into references if response type is DEFINITION
     * @param index - index of definition object in response array to obtain
     * @param start_line - starting line for definition ( zero-based )
     * @param start_character - starting column for definition ( zero-based )
     * @param end_line - ending line for definition ( zero-based )
     * @param end_character - ending column for definition ( zero-based )
     * @return - true if response type right / index valid / parameters are set
     */
    bool getDefinitionAt( int                           index      ,
                          wasp::lsp::clientDefinition & definition );

    /** get hover text string stored for response if HOVER is response type
     * @param hover_text - string reference to fill with text from response
     * @return - true if response type is HOVER / id matches / text was set
     */
    bool getHoverText(std::string & hover_text);

    /** get parameters at index into references if response type is REFERENCES
     * @param index - index of references object in response array to obtain
     * @param start_line - starting line for reference ( zero-based )
     * @param start_character - starting column for reference ( zero-based )
     * @param end_line - ending line for reference ( zero-based )
     * @param end_character - ending column for reference ( zero-based )
     * @return - true if response type right / index valid / parameters are set
     */
    bool getReferencesAt( int                          index      ,
                          wasp::lsp::clientReference & references );

    /** get parameters at index into references if response type is FORMATTING
     * @param index - index of text edit object in response array to obtain
     * @param start_line - starting line for text edit ( zero-based )
     * @param start_character - starting column for text edit ( zero-based )
     * @param end_line - ending line for text edit ( zero-based )
     * @param end_character - ending column for text edit ( zero-based )
     * @param new_text - new text that belongs over the given range
     * @return - true if response type right / index valid / parameters are set
     */
    bool getFormattingAt( int                           index      ,
                          wasp::lsp::clientFormatting & formatting );

    /** build extension request / write to connection / get back response
     * @param line - zero-based input line number to use for this request
     * @param character - zero-based input column to use for this request
     * @return - true if build / write / response read handled successful
     * @return - true if successful request build / write / response read
     */
    bool doExtensionMethod( const std::string & extension_method ,
                            int                 line             ,
                            int                 character        );

    /** get size of result currently stored if EXTENSION is response type
     * @return - size of result list if current response is expected type
     */
    int getExtensionResponseSize();

    /** get extension response stored at given index if EXTENSION is type
     * @param extension_response - object to fill with extension response
     * @return - true if response type is EXTENSION and index is in range
     */
    bool getExtensionResponseAt( int                index             ,
                                 wasp::DataObject & extension_response );

    /** handle unregister and register requests by server for watch files
     ** this is called by LSPInterpreter if server has capability enabled
     * @return - true if unregister and register requests were successful
     */
    bool handleWatchFileRegistration();

    /** get set of all current files that client should watch for changes
     ** this can be called by Workbench to get list of all files to watch
     * @return - set of files that client should tell server when changed
     */
    std::set<std::string> getAllWatchFiles() const;

    /** notify server about any changed resources and get all diagnostics
     ** this can be called by Workbench when any of it watch files change
     * @param resource_uris - set of uris for resources that have changed
     * @param all_diagnostics - map of file uris to clientDiagnostic list
     * @return - true if notification and diagnostics read was successful
     */
    bool notifyServerChangedWatchedFiles(
        const std::set<std::string>                          & resource_uris   ,
        std::map<std::string, std::vector<clientDiagnostic>> & all_diagnostics );

    /** check if the client is properly connected for reading / writing
     * @return - true if the client is properly connected for reading / writing
     */
    bool isConnected()
    {
        return this->is_connected;
    }

    /** get a shared pointer to this client's read / write connection
     * @return - shared pointer to the client's read / write connection
     */
    wasp::lsp::Connection::SP getConnection()
    {
        return this->connection;
    }

    /** get all errors that have been stored on the client for any reason
     * @return - error string of all errors that have been stored on the client
     */
    std::string getErrors() const
    {
        return this->errors.str();
    }

    /** get previous request id used by the client for a request to the server
     * @return - integer id of the previous request to the server
     */
    int getPreviousRequestID() const
    {
        return this->request_id;
    }

    /** check if the client currently has a document open
     * @return - true if client currently has a document open
     */
    bool isDocumentOpen() const
    {
        return this->is_document_open;
    }

    /** get the document URI of the currently open document on the client
     * @return - document URI string of the open document on the client
     */
    std::string getDocumentPath() const
    {
        return this->document_path;
    }

    /** get current document version number which should increase on each change
     * @return - integer id of the current document version number
     */
    int getCurrentDocumentVersion() const
    {
        return this->document_version;
    }

    /** get a shared pointer to a symbol iterator for the client symbol response
     * @return - shared pointer to a symbol iterator
     */
    wasp::lsp::SymbolIterator::SP getSymbolIterator()
    {
        wasp_check( verifySymbolsResponse( *(this->response) ) );

        return std::make_shared<wasp::lsp::SymbolIterator>( this->response );
    }

  private:

      /**
       * @brief struct existing in a scope which sets / unsets already_in_call
       */
      struct InsideClientCall
      {
          InsideClientCall( bool & in_call ) : in_call( in_call )
          {
              this->in_call = true;
          }
          ~InsideClientCall()
          {
              this->in_call = false;
          }
          bool & in_call;
      };

      /**
       * @brief shared pointer to this client's read / write connection
       */
      wasp::lsp::Connection::SP connection;

      /**
       * @brief errors - all errors stored by the client for any reason
       */
      std::stringstream errors;

      /**
       * @brief is_connected - has the client read / write connection been set
       */
      bool is_connected;

      /**
       * @brief is_initialized - has the protocol initialize been handled yet
       */
      bool is_initialized;

      /**
       * @brief is_document_open - is there a document open on the client
       */
      bool is_document_open;

      /**
       * @brief already_in_call - is this client in a method on another thread
       */
      bool already_in_call;

      /**
       * @brief request_id - request id of most recent request to server
       */
      int request_id;

      /**
       * @brief document_version - document version incremented for each change
       */
      int document_version;

      /**
       * @brief document_path - document URI of the open document on the client
       */
      std::string document_path;

      /**
       * @brief support_snippets - indicates if autocompletion snippets are a supported client capability
       */
      bool support_snippets;

      /**
       * @brief support_watchers - client allows watch files registration
       */
      bool support_watchers;

      /**
       * @brief client_extension_methods - extension methods enabled on client
       */
      std::set<std::string> client_extension_methods;

      /**
       * @brief server_extension_methods - extension methods enabled on server
       */
      std::set<std::string> server_extension_methods;

      /**
       * @brief enum list indicating the currently stored response type
       */
      enum {
        INITIALIZE ,
        DIAGNOSTIC ,
        COMPLETION ,
        DEFINITION ,
        HOVER      ,
        REFERENCES ,
        FORMATTING ,
        SYMBOLS    ,
        EXTENSION  ,
        SHUTDOWN   ,
        NONE
      } response_type;

      /**
       * @brief shared pointer to the currently stored response
       */
      wasp::DataObject::SP response;

      /**
       * @brief increment the current request id for each server request
       */
      void incrementRequestID()
      {
          this->request_id++;
      }

      /**
       * @brief increment the current document version for each document change
       */
      void incrementDocumentVersion()
      {
          this->document_version++;
      }

      /**
       * @brief watch_files - registration ids to sets of watch files map
       */
      std::map<std::string, std::set<std::string>> watch_files;
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_CLIENT_IMPL_H
