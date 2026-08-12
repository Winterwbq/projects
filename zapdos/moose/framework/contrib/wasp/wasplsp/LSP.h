#ifndef WASPLSP_LSP_H
#define WASPLSP_LSP_H

#include <string>
#include <iostream>
#include <set>
#include <map>
#include "waspcore/Object.h"
#include "waspcore/decl.h"

namespace wasp {
namespace lsp  {

/** build a proper JSON-RPC string for the LSP from the provided object
 * @param object - reference to object that will be converted to JSON-RPC string
 * @param rpcstr - reference to string that will be filled from provided object
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully converted to JSON-RPC string
 */
WASP_PUBLIC
bool objectToRPCString( wasp::DataObject & object ,
                        std::string  & rpcstr ,
                        std::ostream & errors );

/** build a DataObject from the provided LSP JSON-RPC string
 * @param rpcstr - reference to JSON-RPC string that will be converted to object
 * @param object - reference to object that will be filled from conversion
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the JSON-RPC string was successfully converted to object
 */
WASP_PUBLIC
bool RPCStringToObject( const std::string  & rpcstr ,
                              wasp::DataObject & object ,
                              std::ostream & errors );

/** check that the position's line and character numbers are not negative
 * @param errors - reference to stream to fill with any possible errors
 * @param line - line number ( zero-based )
 * @param character - column number ( zero-based )
 * @return - true if the position's line and character values are non negative
 */
WASP_PUBLIC
bool checkPosition( std::ostream & errors    ,
                    int            line      ,
                    int            character );

/** check that the range's end line / char is not before start line / char
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number ( zero-based )
 * @param start_character - starting column number ( zero-based )
 * @param end_line - ending line number ( zero-based )
 * @param end_character - ending column number ( zero-based )
 * @return - true if the range's start and end positions are valid
 */
WASP_PUBLIC
bool checkRange( std::ostream & errors          ,
                 int            start_line      ,
                 int            start_character ,
                 int            end_line        ,
                 int            end_character   );

/** build position object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param line - line number ( zero-based )
 * @param character - column number ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildPositionObject( wasp::DataObject & object ,
                          std::ostream & errors    ,
                          int            line      ,
                          int            character );

/** dissect position object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param line - line number ( zero-based )
 * @param character - column number ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectPositionObject( const wasp::DataObject & object ,
                                  std::ostream & errors    ,
                                  int          & line      ,
                                  int          & character );

/** build range object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number ( zero-based )
 * @param start_character - starting column number ( zero-based )
 * @param end_line - ending line number ( zero-based )
 * @param end_character - ending column number ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildRangeObject( wasp::DataObject & object      ,
                       std::ostream & errors          ,
                       int            start_line      ,
                       int            start_character ,
                       int            end_line        ,
                       int            end_character   );

/** dissect range object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number ( zero-based )
 * @param start_character - starting column number ( zero-based )
 * @param end_line - ending line number ( zero-based )
 * @param end_character - ending column number ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectRangeObject( const wasp::DataObject & object         ,
                               std::ostream    & errors          ,
                               int             & start_line      ,
                               int             & start_character ,
                               int             & end_line        ,
                               int             & end_character   );

/** build initialize request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param process_id - process id value ( -1 for null value )
 * @param root_uri - root workspace folder URI ( empty for null value )
 * @param client_capabilities - client capabilities object
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildInitializeRequest( wasp::DataObject  & object              ,
                             std::ostream      & errors              ,
                             int                 request_id          ,
                             int                 process_id          ,
                             const std::string & root_uri            ,
                             const wasp::DataObject & client_capabilities );

/** dissect initialize request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param process_id - process id value
 * @param root_uri - root workspace folder URI ( empty for null value )
 * @param client_capabilities - client capabilities object
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectInitializeRequest( const wasp::DataObject  & object              ,
                                     std::ostream      & errors              ,
                                     int               & request_id          ,
                                     int               & process_id          ,
                                     std::string       & root_uri            ,
                                     wasp::DataObject  & client_capabilities );

/** build initialized notification object ( should be empty )
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildInitializedNotification( wasp::DataObject & object ,
                                   std::ostream & errors );

/** dissect initialized notification object ( should be empty )
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectInitializedNotification( const wasp::DataObject & object ,
                                           std::ostream & errors );

/** build didopen notification object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param language_id - document language id string
 * @param version - document version ( increases for each change )
 * @param text - full text of the document
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDidOpenNotification( wasp::DataObject  & object      ,
                               std::ostream      & errors      ,
                               const std::string & uri         ,
                               const std::string & language_id ,
                               int                 version     ,
                               const std::string & text        );

/** dissect didopen notification object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param language_id - document language id string
 * @param version - document version ( increases for each change )
 * @param text - full text of the document
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDidOpenNotification( const wasp::DataObject & object  ,
                                       std::ostream & errors      ,
                                       std::string  & uri         ,
                                       std::string  & language_id ,
                                       int          & version     ,
                                       std::string  & text        );

/** build didchange notification object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param version - document version ( increases for each change )
 * @param start_line - starting line number of change ( zero-based )
 * @param start_character - starting column number of change ( zero-based )
 * @param end_line - ending line number of change ( zero-based )
 * @param end_character - ending column number of change ( zero-based )
 * @param range_length - range length of document change
 * @param text - text of the document change over the provided range
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDidChangeNotification( wasp::DataObject  & object          ,
                                 std::ostream      & errors          ,
                                 const std::string & uri             ,
                                 int                 version         ,
                                 int                 start_line      ,
                                 int                 start_character ,
                                 int                 end_line        ,
                                 int                 end_character   ,
                                 int                 range_length    ,
                                 const std::string & text            );

/** dissect didchange notification object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param version - document version ( increases for each change )
 * @param start_line - starting line number of change ( zero-based )
 * @param start_character - starting column number of change ( zero-based )
 * @param end_line - ending line number of change ( zero-based )
 * @param end_character - ending column number of change ( zero-based )
 * @param range_length - range length of document change
 * @param text - text of the document change over the provided range
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDidChangeNotification( const wasp::DataObject & object      ,
                                         std::ostream & errors          ,
                                         std::string  & uri             ,
                                         int          & version         ,
                                         int          & start_line      ,
                                         int          & start_character ,
                                         int          & end_line        ,
                                         int          & end_character   ,
                                         int          & range_length    ,
                                         std::string  & text            );

/** build didchangewatchedfiles notification to server that files changed
 * @param object - reference to object that will be built of notification
 * @param errors - reference to stream to add any possible error messages
 * @param resource_uris - set of all uris for resources that have changed
 * @return - true if notification object successfully built without error
 */

WASP_PUBLIC
bool buildDidChangeWatchedFilesNotification( wasp::DataObject            & object        ,
                                             std::ostream                & errors        ,
                                             const std::set<std::string> & resource_uris );

/** dissect didchangewatchedfiles notification to get changed files array
 * @param object - const reference to notification object being dissected
 * @param errors - reference to stream to add any possible error messages
 * @param changes - reference for array to get filled with change objects
 * @return - true if notification object was dissected without any errors
 */
WASP_PUBLIC
bool dissectDidChangeWatchedFilesNotification( const wasp::DataObject & object  ,
                                                     std::ostream     & errors  ,
                                                     wasp::DataArray  & changes );

/** dissect object in didchangewatchedfiles notification array of changes
 * @param object - const reference to array object that will be dissected
 * @param errors - reference to stream to add any possible error messages
 * @param uri - well formed URI path indicating location for changed file
 * @param type - type of file change: created (1) changed (2) deleted (3)
 * @return - true if change array object was dissected without any errors
 */
WASP_PUBLIC
bool dissectChangeObject( const wasp::DataObject & object ,
                                std::ostream     & errors ,
                                std::string      & uri    ,
                                int              & type   );

/** build logmessage notification that indicates end of diagnostics array
 * @param object - reference to object that will be built of notification
 * @param errors - reference to stream to add any possible error messages
 * @return - true if notification object successfully built without error
 */
WASP_PUBLIC
bool buildDiagnosticsSentinelObject( wasp::DataObject  & object ,
                                     std::ostream      & errors );

/** check if provided object is sentinel terminating array of diagnostics
 * @param object - object that will get checked to know if it is sentinel
 * @return - true if object is sentinel that terminates diagnostics array
 */
WASP_PUBLIC
bool isDiagnosticsSentinelObject( const DataObject & object );

/** build request to unregister client didChangeWatchedFiles for document
 * @param object - reference to data object that will be built of request
 * @param errors - reference to stream to add any possible error messages
 * @param request_id - id of server request to match response from client
 * @param uri - well formed URI path for input document from this request
 * @return - true if request object successfully built without any errors
 */
WASP_PUBLIC
bool buildUnregisterWatchFilesRequest( wasp::DataObject  & object     ,
                                       std::ostream      & errors     ,
                                       int                 request_id ,
                                       const std::string & uri        );

/** dissect watched files unregisterCapability request into useful pieces
 * @param object - const reference to request data that will be dissected
 * @param errors - reference to stream to add any possible error messages
 * @param request_id - id of server request to match response from client
 * @param registration_ids - set to be filled with ids for unregistration
 * @return - true if request was dissected successfully and without error
 */
WASP_PUBLIC
bool dissectUnregisterWatchFilesRequest( const wasp::DataObject      & object           ,
                                               std::ostream          & errors           ,
                                               int                   & request_id       ,
                                               std::set<std::string> & registration_ids );

/** build request to register client didChangeWatchedFiles with resources
 * @param object - reference to data object that will be built of request
 * @param errors - reference to stream to add any possible error messages
 * @param request_id - id of server request to match response from client
 * @param uri - well formed URI path for input document from this request
 * @param resource_uris - set of resource uris that client needs to watch
 * @return - true if request object successfully built without any errors
 */
WASP_PUBLIC
bool buildRegisterWatchFilesRequest( wasp::DataObject            & object        ,
                                     std::ostream                & errors        ,
                                     int                           request_id    ,
                                     const std::string           & uri           ,
                                     const std::set<std::string> & resource_uris );

/** dissect watched files registerCapability request for given parameters
 * @param object - const reference to request data that will be dissected
 * @param errors - reference to stream to add any possible error messages
 * @param request_id - id of server request to match response from client
 * @param keyed_resources - map of registration id to watch files to fill
 * @return - true if request was dissected successfully and without error
 */
WASP_PUBLIC
bool dissectRegisterWatchFilesRequest(
    const wasp::DataObject                             & object          ,
          std::ostream                                 & errors          ,
          int                                          & request_id      ,
          std::map<std::string, std::set<std::string>> & keyed_resources );

/** build response to unregister and register requests indicating success
 * @param object - reference to data object that built response will fill
 * @param errors - reference to stream to add any possible error messages
 * @param request_id - id of server request to match response from client
 * @return - true if response object was successfully built without error
 */
WASP_PUBLIC
bool buildRegistrationResponse( wasp::DataObject & object     ,
                                std::ostream     & errors     ,
                                int                request_id );

/** build completion request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildCompletionRequest( wasp::DataObject  & object     ,
                             std::ostream      & errors     ,
                             int                 request_id ,
                             const std::string & uri        ,
                             int                 line       ,
                             int                 character  );

/** dissect completion request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectCompletionRequest( const wasp::DataObject & object ,
                                     std::ostream & errors     ,
                                     int          & request_id ,
                                     std::string  & uri        ,
                                     int          & line       ,
                                     int          & character  );

/** build definition request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDefinitionRequest( wasp::DataObject  & object     ,
                             std::ostream      & errors     ,
                             int                 request_id ,
                             const std::string & uri        ,
                             int                 line       ,
                             int                 character );

/** dissect definition request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDefinitionRequest( const wasp::DataObject & object ,
                                     std::ostream & errors     ,
                                     int          & request_id ,
                                     std::string  & uri        ,
                                     int          & line       ,
                                     int          & character  );

/** build hover request object using provided parameters for input location
 * @param object - reference to data object that will be built with request
 * @param errors - reference to stream to fill with possible error messages
 * @param request_id - id from client request to match response from server
 * @param uri - well formatted URI path of input document from this request
 * @param line - zero-based line number in input document from this request
 * @param character - zero-based column in input document from this request
 * @return - true if successful and built request object without any errors
 */
WASP_PUBLIC
bool buildHoverRequest( wasp::DataObject  & object     ,
                        std::ostream      & errors     ,
                        int                 request_id ,
                        const std::string & uri        ,
                        int                 line       ,
                        int                 character );

/** dissect hover request object into provided document location parameters
 * @param object - const reference to request object that will be dissected
 * @param errors - reference to stream to fill with possible error messages
 * @param request_id - id from client request to match response from server
 * @param uri - well formatted URI path of input document from this request
 * @param line - zero-based line number in input document from this request
 * @param character - zero-based column in input document from this request
 * @return - true if successful and dissected request object without errors
 */
WASP_PUBLIC
bool dissectHoverRequest( const wasp::DataObject & object ,
                          std::ostream & errors     ,
                          int          & request_id ,
                          std::string  & uri        ,
                          int          & line       ,
                          int          & character  );

/** build references request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @param include_declaration - flag indicating whether to include declaration
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildReferencesRequest( wasp::DataObject  & object              ,
                             std::ostream      & errors              ,
                             int                 request_id          ,
                             const std::string & uri                 ,
                             int                 line                ,
                             int                 character           ,
                             bool                include_declaration );

/** dissect definition request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param line - line number of request ( zero-based )
 * @param character - column number of request ( zero-based )
 * @param include_declaration - flag indicating whether to include declaration
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectReferencesRequest( const wasp::DataObject & object          ,
                                     std::ostream & errors              ,
                                     int          & request_id          ,
                                     std::string  & uri                 ,
                                     int          & line                ,
                                     int          & character           ,
                                     bool         & include_declaration );

/** build formatting request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param tab_size - value of the size of a tab in spaces for formatting
 * @param insert_spaces - flag indicating whether to use spaces for tabs
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildFormattingRequest( wasp::DataObject  & object          ,
                             std::ostream      & errors          ,
                             int                 request_id      ,
                             const std::string & uri             ,
                             int                 tab_size        ,
                             bool                insert_spaces   );

/** dissect formatting request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @param tab_size - value of the size of a tab in spaces for formatting
 * @param insert_spaces - flag indicating whether to use spaces for tabs
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectFormattingRequest( const wasp::DataObject & object      ,
                                     std::ostream & errors          ,
                                     int          & request_id      ,
                                     std::string  & uri             ,
                                     int          & tab_size        ,
                                     bool         & insert_spaces   );

/** build symbols request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildSymbolsRequest( wasp::DataObject  & object     ,
                          std::ostream      & errors     ,
                          int                 request_id ,
                          const std::string & uri        );

/** dissect symbols request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param uri - document URI ( should be in proper URI format )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectSymbolsRequest( const wasp::DataObject & object ,
                                  std::ostream & errors     ,
                                  int          & request_id ,
                                  std::string  & uri        );

/** build extension request object from parameters with input information
 * @param object - reference to data object that will be built of request
 * @param errors - reference to stream to add any possible error messages
 * @param extension_method - name given to custom server extension method
 * @param request_id - id of client request to match response from server
 * @param uri - well formed URI path for input document from this request
 * @param line - zero-based line number of document for extension request
 * @param character - zero-based column of document for extension request
 * @return - true if request object successfully built without any errors
 */
WASP_PUBLIC
bool buildExtensionRequest( wasp::DataObject  & object           ,
                            std::ostream      & errors           ,
                            const std::string & extension_method ,
                            int                 request_id       ,
                            const std::string & uri              ,
                            int                 line             ,
                            int                 character        );

/** dissect custom extension request object data into provided parameters
 * @param object - const reference to request data that will be dissected
 * @param errors - reference to stream to add any possible error messages
 * @param request_id - id of client request to match response from server
 * @param uri - well formed URI path for input document from this request
 * @param line - zero-based line number of document for extension request
 * @param character - zero-based column of document for extension request
 * @return - true if request was dissected into parameters without errors
 */
WASP_PUBLIC
bool dissectExtensionRequest( const wasp::DataObject & object     ,
                                    std::ostream     & errors     ,
                                    int              & request_id ,
                                    std::string      & uri        ,
                                    int              & line       ,
                                    int              & character  );

/** build extension response object from array of results with request id
 * @param object - reference to response data object that will get filled
 * @param errors - reference to stream to add any possible error messages
 * @param request_id - id of client request to match response from server
 * @param extension_responses - data array of results for response object
 * @return - true if reference to response object was filled successfully
 */
WASP_PUBLIC
bool buildExtensionResponse( wasp::DataObject & object              ,
                             std::ostream     & errors              ,
                             int                request_id          ,
                             const wasp::DataArray & extension_responses );

/** dissect extension response object into array of result and request id
 * @param object - const reference to object for response being dissected
 * @param errors - reference to stream to add any possible error messages
 * @param request_id - id of client request to match response from server
 * @param extension_responses - reference for results array to get filled
 * @return - true if response was dissected to results without any errors
 */
WASP_PUBLIC
bool dissectExtensionResponse( const wasp::DataObject & object              ,
                                     std::ostream     & errors              ,
                                     int              & request_id          ,
                                     wasp::DataArray  & extension_responses );

/** verify if provided object structure is correct for extension response
 * @param object - const reference to object for response to get verified
 * @return - true if structure of object is proper for extension response
 */
WASP_PUBLIC
bool verifyExtensionResponse( const wasp::DataObject & object );

/** return data array pointer to custom extension method response objects
 * @param object - parent object for which array pointer will be returned
 * @return - data array pointer to response objects from custom extension
 */
WASP_PUBLIC
wasp::DataArray * getExtensionResponseArray( const wasp::DataObject & object );

/** build didclose notification object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDidCloseNotification( wasp::DataObject  & object ,
                                std::ostream      & errors ,
                                const std::string & uri    );

/** dissect didclose notification object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDidCloseNotification( const wasp::DataObject & object ,
                                        std::ostream & errors ,
                                        std::string  & uri    );

/** build shutdown request object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildShutdownRequest( wasp::DataObject & object ,
                           std::ostream & errors     ,
                           int            request_id );

/** dissect shutdown request object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectShutdownRequest( const wasp::DataObject & object ,
                                   std::ostream & errors     ,
                                   int          & request_id );

/** build exit notification object ( should be empty )
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildExitNotification( wasp::DataObject & object ,
                            std::ostream & errors );

/** dissect exit notification object ( should be empty )
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectExitNotification( const wasp::DataObject & object ,
                                    std::ostream & errors );

/** build diagnostic object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number of diagnostic ( zero-based )
 * @param start_character - starting column number of diagnostic ( zero-based )
 * @param end_line - ending line number of diagnostic ( zero-based )
 * @param end_character - ending column number of diagnostic ( zero-based )
 * @param severity - diagnostic severity number
 * @param code - reference to diagnostic code string
 * @param source - reference to diagnostic source string
 * @param message - reference to diagnostic message string
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDiagnosticObject( wasp::DataObject  & object          ,
                            std::ostream      & errors          ,
                            int                 start_line      ,
                            int                 start_character ,
                            int                 end_line        ,
                            int                 end_character   ,
                            int                 severity        ,
                            const std::string & code            ,
                            const std::string & source          ,
                            const std::string & message         );

/** dissect diagnostic object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number of diagnostic ( zero-based )
 * @param start_character - starting column number of diagnostic ( zero-based )
 * @param end_line - ending line number of diagnostic ( zero-based )
 * @param end_character - ending column number of diagnostic ( zero-based )
 * @param severity - diagnostic severity number
 * @param code - reference to diagnostic code string
 * @param source - reference to diagnostic source string
 * @param message - reference to diagnostic message string
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDiagnosticObject( const wasp::DataObject & object      ,
                                    std::ostream & errors          ,
                                    int          & start_line      ,
                                    int          & start_character ,
                                    int          & end_line        ,
                                    int          & end_character   ,
                                    int          & severity        ,
                                    std::string  & code            ,
                                    std::string  & source          ,
                                    std::string  & message         );

/** build publish diagnostics notification object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param diagnostics - data array of diagnostic data objects
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildPublishDiagnosticsNotification( wasp::DataObject  & object      ,
                                          std::ostream      & errors      ,
                                          const std::string & uri         ,
                                          const wasp::DataArray & diagnostics );

/** dissect publish diagnostics notification into the parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI ( should be in proper URI format )
 * @param diagnostics - data array of diagnostic data objects
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectPublishDiagnosticsNotification( const wasp::DataObject & object  ,
                                                  std::ostream & errors      ,
                                                  std::string  & uri         ,
                                                  wasp::DataArray & diagnostics );

/** build initialize response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param server_capabilities - server capabilities object
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildInitializeResponse( wasp::DataObject  & object              ,
                              std::ostream      & errors              ,
                              int                 request_id          ,
                              const wasp::DataObject & server_capabilities );

/** dissect initialize response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param server_capabilities - server capabilities object
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectInitializeResponse( const wasp::DataObject & object          ,
                                      std::ostream & errors              ,
                                      int          & request_id          ,
                                      wasp::DataObject & server_capabilities );

/** build completion item object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param label - label to be used for this item in autocomplete list
 * @param start_line - starting line number for this item ( zero-based )
 * @param start_character - starting column number for this item ( zero-based )
 * @param end_line - ending line number for this item ( zero-based )
 * @param end_character - ending column number for this item ( zero-based )
 * @param new_text - text to be inserted for this item upon autocomplete
 * @param kind - kind value for this item
 * @param detail - detail string for this item
 * @param documentation - documentation string for this item
 * @param deprecated - flag indicating if this item is deprecated
 * @param preselect - flag indicating if this item should be pre-selected
 * @param insert_text_format - plain text or snippet format for new_text
                               defaults to plain text when not specified
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildCompletionObject( wasp::DataObject  & object          ,
                            std::ostream      & errors          ,
                            const std::string & label           ,
                            int                 start_line      ,
                            int                 start_character ,
                            int                 end_line        ,
                            int                 end_character   ,
                            const std::string & new_text        ,
                            int                 kind            ,
                            const std::string & detail          ,
                            const std::string & documentation   ,
                            bool                deprecated      ,
                            bool                preselect       ,
                            int                 insert_text_format = 1); // m_text_format_plaintext

/** dissect completion item object into the provided parameter references
 ** this version accepts insert_text_format to fill if in the data object
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param label - label to be used for this item in autocomplete list
 * @param start_line - starting line number for this item ( zero-based )
 * @param start_character - starting column number for this item ( zero-based )
 * @param end_line - ending line number for this item ( zero-based )
 * @param end_character - ending column number for this item ( zero-based )
 * @param new_text - text to be inserted for this item upon autocomplete
 * @param kind - kind value for this item
 * @param detail - detail string for this item
 * @param documentation - documentation string for this item
 * @param deprecated - flag indicating if this item is deprecated
 * @param preselect - flag indicating if this item should be pre-selected
 * @param insert_text_format - plain text or snippet format for new_text
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectCompletionObject( const wasp::DataObject & object      ,
                                    std::ostream & errors          ,
                                    std::string  & label           ,
                                    int          & start_line      ,
                                    int          & start_character ,
                                    int          & end_line        ,
                                    int          & end_character   ,
                                    std::string  & new_text        ,
                                    int          & kind            ,
                                    std::string  & detail          ,
                                    std::string  & documentation   ,
                                    bool         & deprecated      ,
                                    bool         & preselect       ,
                                    int          & insert_text_format );

/** dissect completion item object into the provided parameter references
 ** this version without insert_text_format ignores that object parameter
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param label - label to be used for this item in autocomplete list
 * @param start_line - starting line number for this item ( zero-based )
 * @param start_character - starting column number for this item ( zero-based )
 * @param end_line - ending line number for this item ( zero-based )
 * @param end_character - ending column number for this item ( zero-based )
 * @param new_text - text to be inserted for this item upon autocomplete
 * @param kind - kind value for this item
 * @param detail - detail string for this item
 * @param documentation - documentation string for this item
 * @param deprecated - flag indicating if this item is deprecated
 * @param preselect - flag indicating if this item should be pre-selected
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectCompletionObject( const wasp::DataObject & object      ,
                                    std::ostream & errors          ,
                                    std::string  & label           ,
                                    int          & start_line      ,
                                    int          & start_character ,
                                    int          & end_line        ,
                                    int          & end_character   ,
                                    std::string  & new_text        ,
                                    int          & kind            ,
                                    std::string  & detail          ,
                                    std::string  & documentation   ,
                                    bool         & deprecated      ,
                                    bool         & preselect       );

/** build completion response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param is_incomplete - flag indicating if the completion items are complete
 * @param completion_items - data array of completion item data objects
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildCompletionResponse( wasp::DataObject  & object           ,
                              std::ostream      & errors           ,
                              int                 request_id       ,
                              bool                is_incomplete    ,
                              const wasp::DataArray & completion_items );

/** dissect completion response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param is_incomplete - flag indicating if the completion items are complete
 * @param completion_items - data array of completion item data objects
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectCompletionResponse( const wasp::DataObject & object       ,
                                      std::ostream & errors           ,
                                      int          & request_id       ,
                                      bool         & is_incomplete    ,
                                      wasp::DataArray & completion_items );

/** build location object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI for location ( should be in proper URI format )
 * @param start_line - starting line number for location ( zero-based )
 * @param start_character - starting column number for location ( zero-based )
 * @param end_line - ending line number for location ( zero-based )
 * @param end_character - ending column number for location ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildLocationObject( wasp::DataObject  & object          ,
                          std::ostream      & errors          ,
                          const std::string & uri             ,
                          int                 start_line      ,
                          int                 start_character ,
                          int                 end_line        ,
                          int                 end_character   );

/** dissect location object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param uri - document URI for location ( should be in proper URI format )
 * @param start_line - starting line number for location ( zero-based )
 * @param start_character - starting column number for location ( zero-based )
 * @param end_line - ending line number for location ( zero-based )
 * @param end_character - ending column number for location ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectLocationObject( const wasp::DataObject & object      ,
                                  std::ostream & errors          ,
                                  std::string  & uri             ,
                                  int          & start_line      ,
                                  int          & start_character ,
                                  int          & end_line        ,
                                  int          & end_character   );

/** build locations response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param location_objects - data array of location data objects
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildLocationsResponse( wasp::DataObject  & object           ,
                             std::ostream      & errors           ,
                             int                 request_id       ,
                             const wasp::DataArray & location_objects );

/** dissect locations response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param location_objects - data array of location data objects
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectLocationsResponse( const wasp::DataObject & object       ,
                                     std::ostream & errors           ,
                                     int          & request_id       ,
                                     wasp::DataArray & location_objects );

/** build hover response object from provided text that should be displayed
 * @param object - reference to data object that will be built for response
 * @param errors - reference to stream to fill with possible error messages
 * @param request_id - id from client request to match response from server
 * @param display_text - text to display for hover at location from request
 * @return - true if response object reference was built without any errors
 */
WASP_PUBLIC
bool buildHoverResponse( wasp::DataObject  & object       ,
                         std::ostream      & errors       ,
                         int                 request_id   ,
                         const std::string & display_text );

/** dissect hover response object into provided string reference to display
 * @param object - const reference to object for response getting dissected
 * @param errors - reference to stream to fill with possible error messages
 * @param request_id - id from client request to match response from server
 * @param display_text - text to display for hover at location from request
 * @return - true if response object reference was dissected without errors
 */
WASP_PUBLIC
bool dissectHoverResponse( const wasp::DataObject  & object       ,
                                 std::ostream      & errors       ,
                                 int               & request_id   ,
                                 std::string       & display_text );

/** build text edit object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number for text edit ( zero-based )
 * @param start_character - starting column number for text edit ( zero-based )
 * @param end_line - ending line number for text edit ( zero-based )
 * @param end_character - ending column number for text edit ( zero-based )
 * @param new_text - new text that belongs over the given range
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildTextEditObject( wasp::DataObject  & object          ,
                          std::ostream      & errors          ,
                          int                 start_line      ,
                          int                 start_character ,
                          int                 end_line        ,
                          int                 end_character   ,
                          const std::string & new_text        );

/** dissect text edit object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param start_line - starting line number for text edit ( zero-based )
 * @param start_character - starting column number for text edit ( zero-based )
 * @param end_line - ending line number for text edit ( zero-based )
 * @param end_character - ending column number for text edit ( zero-based )
 * @param new_text - new text that belongs over the given range
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectTextEditObject( const wasp::DataObject & object      ,
                                  std::ostream & errors          ,
                                  int          & start_line      ,
                                  int          & start_character ,
                                  int          & end_line        ,
                                  int          & end_character   ,
                                  std::string  & new_text        );

/** build formatting response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param textedit_objects - data array of text edit data objects
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildFormattingResponse( wasp::DataObject  & object           ,
                              std::ostream      & errors           ,
                              int                 request_id       ,
                              const wasp::DataArray & textedit_objects );

/** dissect formatting response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param textedit_objects - data array of text edit data objects
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectFormattingResponse( const wasp::DataObject & object       ,
                                      std::ostream & errors           ,
                                      int          & request_id       ,
                                      wasp::DataArray & textedit_objects );

/** build document symbol object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param name - name of this document symbol
 * @param detail - detail string for this document symbol
 * @param kind - kind value for this document symbol
 * @param deprecated - flag indicating if this document symbol is deprecated
 * @param start_line - starting line number for symbol ( zero-based )
 * @param start_character - starting column number for symbol ( zero-based )
 * @param end_line - ending line number for symbol ( zero-based )
 * @param end_character - ending column number for symbol ( zero-based )
 * @param selection_start_line - starting selection line ( zero-based )
 * @param selection_start_character - starting selection column ( zero-based )
 * @param selection_end_line - ending selection line ( zero-based )
 * @param selection_end_character - ending selection column ( zero-based )
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildDocumentSymbolObject( wasp::DataObject  & object                    ,
                                std::ostream      & errors                    ,
                                const std::string & name                      ,
                                const std::string & detail                    ,
                                int                 kind                      ,
                                bool                deprecated                ,
                                int                 start_line                ,
                                int                 start_character           ,
                                int                 end_line                  ,
                                int                 end_character             ,
                                int                 selection_start_line      ,
                                int                 selection_start_character ,
                                int                 selection_end_line        ,
                                int                 selection_end_character   );

/** dissect document symbol object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param name - name of this document symbol
 * @param detail - detail string for this document symbol
 * @param kind - kind value for this document symbol
 * @param deprecated - flag indicating if this document symbol is deprecated
 * @param start_line - starting line number for symbol ( zero-based )
 * @param start_character - starting column number for symbol ( zero-based )
 * @param end_line - ending line number for symbol ( zero-based )
 * @param end_character - ending column number for symbol ( zero-based )
 * @param selection_start_line - starting selection line ( zero-based )
 * @param selection_start_character - starting selection column ( zero-based )
 * @param selection_end_line - ending selection line ( zero-based )
 * @param selection_end_character - ending selection column ( zero-based )
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectDocumentSymbolObject( const wasp::DataObject & object                ,
                                        std::ostream & errors                    ,
                                        std::string  & name                      ,
                                        std::string  & detail                    ,
                                        int          & kind                      ,
                                        bool         & deprecated                ,
                                        int          & start_line                ,
                                        int          & start_character           ,
                                        int          & end_line                  ,
                                        int          & end_character             ,
                                        int          & selection_start_line      ,
                                        int          & selection_start_character ,
                                        int          & selection_end_line        ,
                                        int          & selection_end_character   );

/** append empty symbol object to object's children list and return reference
 * @param parent - the object that will get an empty symbol appended as a child
 * @return - reference to newly added empty document symbol child
 */
WASP_PUBLIC
wasp::DataObject & addDocumentSymbolChild( wasp::DataObject & parent );

/** return reference to document symbol children list of the provided parent
 * @param parent - the object for which the children reference will be returned
 * @return - reference to data array of document symbol data object children
 */
WASP_PUBLIC
wasp::DataArray * getDocumentSymbolChildren( const wasp::DataObject & parent );

/** build symbols response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param document_symbol_objects - data array of document symbol data objects
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildSymbolsResponse( wasp::DataObject & object                 ,
                           std::ostream    & errors                  ,
                           int               request_id              ,
                           const wasp::DataArray & document_symbol_objects );

/** dissect symbols response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @param document_symbol_objects - data array of document symbol data objects
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectSymbolsResponse( const wasp::DataObject & object              ,
                                   std::ostream & errors                  ,
                                   int          & request_id              ,
                                   wasp::DataArray & document_symbol_objects );

/** build shutdown response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildShutdownResponse( wasp::DataObject  & object     ,
                            std::ostream      & errors     ,
                            int                 request_id );

/** dissect shutdown response object into the provided parameter references
 * @param object - const reference to data object that will be dissected
 * @param errors - reference to stream to fill with any possible errors
 * @param request_id - request id as per the protocol
 * @return - true if the object was successfully dissected without error
 */
WASP_PUBLIC
bool dissectShutdownResponse( const wasp::DataObject & object ,
                                    std::ostream & errors     ,
                                    int          & request_id );

/** build error response object from the provided parameters
 * @param object - reference to data object that will be built
 * @param code - error code value
 * @param errors - reference to stream to fill with any possible errors
 * @return - true if the object was successfully built without error
 */
WASP_PUBLIC
bool buildErrorResponse( wasp::DataObject  & object ,
                         int                 code   ,
                         const std::string & errors );

/** check if response contains success result or contains error for failure
 * @param object - const reference to response object that will get checked
 * @param errors - reference to stream for adding message if error response
 * @return - true if response contains result or false if it contains error
 */
WASP_PUBLIC
bool checkErrorResponse( const wasp::DataObject & object ,
                               std::ostream & errors );

/** check if object contains integer request id indicating not notification
 * @param object - const reference to object that will get for id existence
 * @return - true if object does contain request id or false if it does not
 */
WASP_PUBLIC
bool objectHasRequestId(const wasp::DataObject & object);

/** verify if the provided data object is an initialize response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyInitializeResponse( const wasp::DataObject & object );

/** verify if the provided data object is a diagnostic response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyDiagnosticResponse( const wasp::DataObject & object );

/** verify if the provided data object is a completion response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyCompletionResponse( const wasp::DataObject & object );

/** verify if the provided data object is a definition response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyDefinitionResponse( const wasp::DataObject & object );

/** verify if provided data object is proper response object type for hover
 * @param object - const reference to response object that will be verified
 * @return - true if provided data object is proper type of response object
 */
WASP_PUBLIC
bool verifyHoverResponse( const wasp::DataObject & object );

/** verify if the provided data object is a references response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyReferencesResponse( const wasp::DataObject & object );

/** verify if the provided data object is a formatting response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyFormattingResponse( const wasp::DataObject & object );

/** verify if the provided data object is a symbols response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifySymbolsResponse( const wasp::DataObject & object );

/** verify if the provided data object is a shutdown response
 * @param object - const reference to data object that will be verified
 * @return - true if provided data object is the proper type of response
 */
WASP_PUBLIC
bool verifyShutdownResponse( const wasp::DataObject & object );

/** return pointer to the data array of diagnostic response data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of response data objects
 */
WASP_PUBLIC
wasp::DataArray * getDiagnosticResponseArray( const wasp::DataObject & object );

/** return pointer to the data array of completion response data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of response data objects
 */
WASP_PUBLIC
wasp::DataArray * getCompletionResponseArray( const wasp::DataObject & object );

/** return pointer to the data array of definition response data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of response data objects
 */
WASP_PUBLIC
wasp::DataArray * getDefinitionResponseArray( const wasp::DataObject & object );

/** return pointer to the data array of references response data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of response data objects
 */
WASP_PUBLIC
wasp::DataArray * getReferencesResponseArray( const wasp::DataObject & object );

/** return pointer to the data array of formatting response data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of response data objects
 */
WASP_PUBLIC
wasp::DataArray * getFormattingResponseArray( const wasp::DataObject & object );

/** return pointer to the data array of symbol children data objects
 * @param object - the parent for which the data array pointer will be returned
 * @return - pointer to the data array of symbol children data objects
 */
WASP_PUBLIC
wasp::DataArray * getSymbolChildrenArray( const wasp::DataObject & object );

/** return given path string with file:// uri scheme removed if it exists
 * @param path - path to have its file:// uri scheme removed if it exists
 * @return - path string that has file:// uri scheme removed if it exists
 */
WASP_PUBLIC
std::string removeUriScheme(const std::string & path);

/** return given path string with file:// uri scheme added onto its front
 * @param path - path for which file:// uri scheme will be added to front
 * @return - path string that has file:// uri scheme added onto its front
 */
WASP_PUBLIC
std::string prefixUriScheme(const std::string & path);

/**
 * @brief convenience parameter struct used for client getDiagnosticAt call
 */
struct clientDiagnostic
{
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
    int         severity;
    std::string code;
    std::string source;
    std::string message;
};

/**
 * @brief convenience parameter struct used for client getCompletionAt call
 */
struct clientCompletion
{
    std::string label;
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
    std::string new_text;
    int         kind;
    std::string detail;
    std::string documentation;
    bool        deprecated;
    bool        preselect;
    int         insert_text_format;
};

/**
 * @brief convenience parameter struct used for client getDefinitionAt call
 */
struct clientDefinition
{
    std::string target_uri;
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
};

/**
 * @brief convenience parameter struct used for client getDiagnosticAt call
 */
struct clientReference
{
    std::string target_uri;
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
};

/**
 * @brief convenience parameter struct used for client getFormattingAt call
 */
struct clientFormatting
{
    int         start_line;
    int         start_character;
    int         end_line;
    int         end_character;
    std::string new_text;
};

/**
 * @brief static const char strings and ints that are used for many purposes
 */
static const char m_rpc_content_len_key[]   = "Content-Length:";
static const char m_rpc_separator[]         = "\r\n\r\n";
static const char m_rpc_jsonrpc_key[]       = "jsonrpc";
static const char m_uri_prefix[]            = "file://";
static const char m_wasp_language_id[]      = "wasplsp";
static const char m_rpc_jsonrpc_val[]       = "2.0";
static const char m_error_prefix[]          = "Error:: ";
static const char m_id[]                    = "id";
static const char m_method_initialize[]     = "initialize";
static const char m_method_initialized[]    = "initialized";
static const char m_method_didopen[]        = "textDocument/didOpen";
static const char m_method_didchange[]      = "textDocument/didChange";
static const char m_method_completion[]     = "textDocument/completion";
static const char m_method_definition[]     = "textDocument/definition";
static const char m_method_hover[]          = "textDocument/hover";
static const char m_method_references[]     = "textDocument/references";
static const char m_method_formatting[]     = "textDocument/formatting";
static const char m_method_documentsymbol[] = "textDocument/documentSymbol";
static const char m_method_pubdiagnostics[] = "textDocument/publishDiagnostics";
static const char m_method_didclose[]       = "textDocument/didClose";
static const char m_method_didchangewatch[] = "workspace/didChangeWatchedFiles";
static const char m_method_logmessage[]     = "window/logMessage";
static const char m_method_unregistercap[]  = "client/unregisterCapability";
static const char m_method_registercap[]    = "client/registerCapability";
static const char m_method_shutdown[]       = "shutdown";
static const char m_method_exit[]           = "exit";
static const char m_method[]                = "method";
static const char m_params[]                = "params";
static const char m_result[]                = "result";
static const char m_error[]                 = "error";
static const char m_process_id[]            = "processId";
static const char m_root_uri[]              = "rootUri";
static const char m_capabilities[]          = "capabilities";
static const char m_text_document[]         = "textDocument";
static const char m_comp[]                  = "completion";
static const char m_compitem[]              = "completionItem";
static const char m_snip[]                  = "snippetSupport";
static const char m_workspace[]             = "workspace";
static const char m_change_watched_files[]  = "didChangeWatchedFiles";
static const char m_dynamic_registration[]  = "dynamicRegistration";
static const char m_relative_patterns[]     = "relativePatternSupport";
static const char m_uri[]                   = "uri";
static const char m_language_id[]           = "languageId";
static const char m_version[]               = "version";
static const char m_text[]                  = "text";
static const char m_content_changes[]       = "contentChanges";
static const char m_location[]              = "location";
static const char m_range[]                 = "range";
static const char m_start[]                 = "start";
static const char m_end[]                   = "end";
static const char m_line[]                  = "line";
static const char m_character[]             = "character";
static const char m_range_length[]          = "rangeLength";
static const char m_position[]              = "position";
static const char m_context[]               = "context";
static const char m_include_declaration[]   = "includeDeclaration";
static const char m_options[]               = "options";
static const char m_tab_size[]              = "tabSize";
static const char m_insert_spaces[]         = "insertSpaces";
static const char m_severity[]              = "severity";
static const char m_code[]                  = "code";
static const char m_source[]                = "source";
static const char m_message[]               = "message";
static const char m_diagnostics[]           = "diagnostics";
static const char m_label[]                 = "label";
static const char m_text_edit[]             = "textEdit";
static const char m_new_text[]              = "newText";
static const char m_kind[]                  = "kind";
static const char m_detail[]                = "detail";
static const char m_documentation[]         = "documentation";
static const char m_deprecated[]            = "deprecated";
static const char m_preselect[]             = "preselect";
static const char m_insert_text_format[]    = "insertTextFormat";
static const char m_items[]                 = "items";
static const char m_is_incomplete[]         = "isIncomplete";
static const char m_name[]                  = "name";
static const char m_selection_range[]       = "selectionRange";
static const char m_children[]              = "children";
static const char m_document_symbol[]       = "documentSymbol";
static const char m_contents[]              = "contents";
static const char m_hierarchical_symbols[]  = "hierarchicalDocumentSymbolSupport";
static const char m_text_doc_sync[]         = "textDocumentSync";
static const char m_open_close[]            = "openClose";
static const char m_change[]                = "change";
static const char m_completion_provider[]   = "completionProvider";
static const char m_resolve_provider[]      = "resolveProvider";
static const char m_doc_symbol_provider[]   = "documentSymbolProvider";
static const char m_doc_format_provider[]   = "documentFormattingProvider";
static const char m_definition_provider[]   = "definitionProvider";
static const char m_references_provider[]   = "referencesProvider";
static const char m_hover_provider[]        = "hoverProvider";
static const char m_extensions_provider[]   = "extensionsProvider";
static const char m_extensions[]            = "extensions";
static const char m_sentinel_log_message[]  = "diagnostics flush complete";
static const char m_unregistrations[]       = "unregisterations";
static const char m_registrations[]         = "registrations";
static const char m_register_options[]      = "registerOptions";
static const char m_watchers[]              = "watchers";
static const char m_glob_pattern[]          = "globPattern";
static const char m_base_uri[]              = "baseUri";
static const char m_pattern[]               = "pattern";
static const char m_changes[]               = "changes";
static const char m_type[]                  = "type";
static const int  m_change_type_created     =  1;
static const int  m_change_type_changed     =  2;
static const int  m_change_type_deleted     =  3;
static const int  m_watch_kind_create       =  1;
static const int  m_watch_kind_change       =  2;
static const int  m_watch_kind_delete       =  4;
static const int  m_message_type_error      =  1;
static const int  m_message_type_warning    =  2;
static const int  m_message_type_info       =  3;
static const int  m_message_type_log        =  4;
static const int  m_change_full             =  1;
static const int  m_change_incr             =  2;
static const int  m_text_format_plaintext   =  1;
static const int  m_text_format_snippet     =  2;
static const int  m_comp_kind_text          =  1;
static const int  m_comp_kind_method        =  2;
static const int  m_comp_kind_function      =  3;
static const int  m_comp_kind_constructor   =  4;
static const int  m_comp_kind_field         =  5;
static const int  m_comp_kind_variable      =  6;
static const int  m_comp_kind_class         =  7;
static const int  m_comp_kind_interface     =  8;
static const int  m_comp_kind_module        =  9;
static const int  m_comp_kind_property      =  10;
static const int  m_comp_kind_unit          =  11;
static const int  m_comp_kind_value         =  12;
static const int  m_comp_kind_enum          =  13;
static const int  m_comp_kind_keyword       =  14;
static const int  m_comp_kind_snippet       =  15;
static const int  m_comp_kind_color         =  16;
static const int  m_comp_kind_file          =  17;
static const int  m_comp_kind_reference     =  18;
static const int  m_comp_kind_folder        =  19;
static const int  m_comp_kind_enum_member   =  20;
static const int  m_comp_kind_constant      =  21;
static const int  m_comp_kind_struct        =  22;
static const int  m_comp_kind_event         =  23;
static const int  m_comp_kind_operator      =  24;
static const int  m_comp_kind_type_param    =  25;
static const int  m_symbol_kind_file        =  1;
static const int  m_symbol_kind_module      =  2;
static const int  m_symbol_kind_namespace   =  3;
static const int  m_symbol_kind_package     =  4;
static const int  m_symbol_kind_class       =  5;
static const int  m_symbol_kind_method      =  6;
static const int  m_symbol_kind_property    =  7;
static const int  m_symbol_kind_field       =  8;
static const int  m_symbol_kind_constructor =  9;
static const int  m_symbol_kind_enum        =  10;
static const int  m_symbol_kind_interface   =  11;
static const int  m_symbol_kind_function    =  12;
static const int  m_symbol_kind_variable    =  13;
static const int  m_symbol_kind_constant    =  14;
static const int  m_symbol_kind_string      =  15;
static const int  m_symbol_kind_number      =  16;
static const int  m_symbol_kind_boolean     =  17;
static const int  m_symbol_kind_array       =  18;
static const int  m_symbol_kind_object      =  19;
static const int  m_symbol_kind_key         =  20;
static const int  m_symbol_kind_null        =  21;
static const int  m_symbol_kind_enum_member =  22;
static const int  m_symbol_kind_struct      =  23;
static const int  m_symbol_kind_event       =  24;
static const int  m_symbol_kind_operator    =  25;
static const int  m_symbol_kind_type_param  =  26;
static const int  m_parse_error             = -32700;
static const int  m_invalid_request_error   = -32600;
static const int  m_method_not_found_error  = -32601;
static const int  m_invalid_params_error    = -32602;
static const int  m_internal_error          = -32603;

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_LSP_H
