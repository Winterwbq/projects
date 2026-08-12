#ifndef WASPLSP_SERVER_H
#define WASPLSP_SERVER_H

#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "waspcore/Object.h"
#include "waspcore/decl.h"

namespace wasp {
namespace lsp  {

template <class T>
class WASP_PUBLIC Server
{
  public:

    Server(){}

    ~Server(){}

    /** read from the server connection and handle packets until exit or error
     * @return - true if returning on exit call / false if returning on error
     */
    bool run()
    {
        return Impl.run();
    }

    /** handle initialize request creating response in provided reference
     * @param initializeRequest - const reference to request to be handled
     * @param initializeResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleInitializeRequest(
                    const DataObject & initializeRequest  ,
                          DataObject & initializeResponse )
    {
        return Impl.handleInitializeRequest( initializeRequest  ,
                                             initializeResponse );
    }

    /** handle initialized notification - no response expected
     * @param initializedNotification - const reference to notification
     * @return - true if notification was successfully handled
     */
    bool handleInitializedNotification(
                    const DataObject & initializedNotification )
    {
        return Impl.handleInitializedNotification( initializedNotification );
    }

    /** handle didopen notification creating diagnostics in provided reference
     * @param didOpenNotification - const reference to notification
     * @param publishDiagnosticsNotification - reference to diagnostics to fill
     * @return - true if packet was successfully handled and diagnostics filled
     */
    bool handleDidOpenNotification(
                    const DataObject & didOpenNotification            ,
                          DataObject & publishDiagnosticsNotification )
    {
        return Impl.handleDidOpenNotification( didOpenNotification            ,
                                               publishDiagnosticsNotification );
    }

    /** handle didchange notification creating diagnostics in provided reference
     * @param didOpenNotification - const reference to notification
     * @param publishDiagnosticsNotification - reference to diagnostics to fill
     * @return - true if packet was successfully handled and diagnostics filled
     */
    bool handleDidChangeNotification(
                    const DataObject & didChangeNotification          ,
                          DataObject & publishDiagnosticsNotification )
    {
        return Impl.handleDidChangeNotification( didChangeNotification          ,
                                                 publishDiagnosticsNotification );
    }

    /** handle workspace didchangewatchedfiles and fill diagnostics array
     * @param didChangeWatchedFilesNotification - contains modified files
     * @param publishDiagnosticsArray - array to store publishDiagnostics
     * @return - true if everything handled and array built without error
     */
    bool handleDidChangeWatchedFilesNotification(
                    const wasp::DataObject & didChangeWatchedFilesNotification ,
                          wasp::DataArray  & publishDiagnosticsArray           )
    {
        return Impl.handleDidChangeWatchedFilesNotification(
                        didChangeWatchedFilesNotification ,
                        publishDiagnosticsArray           );
    }

    /** handle completion request creating response in provided reference
     * @param completionRequest - const reference to request to be handled
     * @param completionResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleCompletionRequest(
                    const DataObject & completionRequest  ,
                          DataObject & completionResponse )
    {
        return Impl.handleCompletionRequest( completionRequest  ,
                                             completionResponse );
    }

    /** handle definition request creating response in provided reference
     * @param definitionRequest - const reference to request to be handled
     * @param definitionResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleDefinitionRequest(
                    const DataObject & definitionRequest  ,
                          DataObject & definitionResponse )
    {
        return Impl.handleDefinitionRequest( definitionRequest  ,
                                             definitionResponse );
    }

    /** handle hover request building response in provided object reference
     * @param hoverRequest - const reference to object for hover parameters
     * @param hoverResponse - object reference where response will be built
     * @return - true if request was handled successfully building response
     */
    bool handleHoverRequest(
                    const DataObject & hoverRequest  ,
                          DataObject & hoverResponse )
    {
        return Impl.handleHoverRequest( hoverRequest  ,
                                        hoverResponse );
    }

    /** handle references request creating response in provided reference
     * @param referencesRequest - const reference to request to be handled
     * @param referencesResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleReferencesRequest(
                    const DataObject & referencesRequest  ,
                          DataObject & referencesResponse )
    {
        return Impl.handleReferencesRequest( referencesRequest  ,
                                             referencesResponse );
    }

    /** handle formatting request creating response in provided reference
     * @param formattingRequest - const reference to request to be handled
     * @param formattingRequest - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleFormattingRequest(
                    const DataObject & formattingRequest  ,
                          DataObject & formattingResponse )
    {
        return Impl.handleFormattingRequest( formattingRequest  ,
                                             formattingResponse );
    }

    /** handle symbols request creating response in provided reference
     * @param symbolsRequest - const reference to request to be handled
     * @param symbolsResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleSymbolsRequest(
                    const DataObject & symbolsRequest  ,
                          DataObject & symbolsResponse )
    {
        return Impl.handleSymbolsRequest( symbolsRequest  ,
                                          symbolsResponse );
    }

    /** handle extension request by adding response to provided reference
     * @param extensionMethod - name for current extension request method
     * @param extensionRequest - const reference to request to be handled
     * @param extensionResponse - reference to object that will be filled
     * @return - true if request successfully handled with response built
     */
    bool handleExtensionRequest(
                    const std::string & extensionMethod   ,
                    const DataObject  & extensionRequest  ,
                          DataObject  & extensionResponse )
    {
        return Impl.handleExtensionRequest( extensionMethod   ,
                                            extensionRequest  ,
                                            extensionResponse );
    }

    /** handle didclose notification - no response expected
     * @param didCloseNotification - const reference to notification
     * @return - true if notification was successfully handled
     */
    bool handleDidCloseNotification(
                    const DataObject & didCloseNotification )
    {
        return Impl.handleDidCloseNotification( didCloseNotification  );
    }

    /** handle shutdown request creating response in provided reference
     * @param shutdownRequest - const reference to request to be handled
     * @param shutdownResponse - reference to object that will hold response
     * @return - true if request was successfully handle and response built
     */
    bool handleShutdownRequest(
                    const DataObject & shutdownRequest  ,
                          DataObject & shutdownResponse )
    {
        return Impl.handleShutdownRequest( shutdownRequest  ,
                                           shutdownResponse );
    }

    /** handle exit notification - no response expected
     * @param exitNotification - const reference to notification
     * @return - true if notification was successfully handled
     */
    bool handleExitNotification(
                    const DataObject & exitNotification )
    {
        return Impl.handleExitNotification( exitNotification  );
    }

    /** register resources for input that client should watch for changes
     * @return - true if unregister and register requests were successful
     */
    bool registerWatchFiles()
    {
        return Impl.registerWatchFiles();
    }

    /** get a shared pointer this server's read / write connection
     * @return - shared pointer to the server's read / write connection
     */
    Connection::SP getConnection()
    {
        return Impl.getConnection();
    }

    /** check if the server is initialized and reading from the connection
     * @return - true if server is initialized and reading from the connection
     */
    bool isRunning()
    {
        return Impl.isRunning();
    }

    /** get all errors that have been stored on the server for any reason
     * @return - error string of all errors that have been stored on the server
     */
    std::string getErrors()
    {
        return Impl.getErrors();
    }

    /** check if client for this server communicated snippet syntax support
     * @return - true if client for this server supports snippet completion
     */
    bool clientSupportsSnippets()
    {
        return Impl.clientSupportsSnippets();
    }

    /** check if client communicated it supports watch files registration
     * @return - true if client supports dynamic watch files registration
     */
    bool clientSupportsWatchers()
    {
        return Impl.clientSupportsWatchers();
    }

    /** get a pointer to server implementation class used for above methods
     * @return - pointer to server implementation class used for above methods
     */
    T * getImpl()
    {
        return &Impl;
    }

  private:

    /**
     * @brief Impl - server implementation that each of the above methods use
     */
    T Impl;
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_SERVER_H
