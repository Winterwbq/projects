#ifndef WASPLSP_TEST_SERVER_H
#define WASPLSP_TEST_SERVER_H

#include <string>
#include <memory>
#include "wasplsp/LSP.h"
#include "wasplsp/ServerImpl.h"
#include "wasplsp/Connection.h"
#include "wasplsp/ThreadConnection.h"
#include "waspcore/Object.h"
#include "waspcore/decl.h"

namespace wasp {
namespace lsp  {

class WASP_PUBLIC TestServer : public ServerImpl
{
  public:

    TestServer()
    {
        // set server connection to a shared pointer to new thread connection
        connection = std::make_shared<ThreadConnection>(this);

        // use methods to enable server capabilities of this implementation
        this->enableFullSync();
        this->enableCompletion();
        this->enableDefinition();
        this->enableFormatting();
        this->enableHover();
        this->enableSymbols();
        this->enableExtension("testMethod01");
        this->enableExtension("testMethod02");
        this->enableExtension("watcherRegistration");
    }

    /** get read / write connection - specific to this server implemention
     * @return - shared pointer to the server's read / write connection
     */
    std::shared_ptr<Connection> getConnection()
    {
        return connection;
    }

  protected:

    /** parse document for diagnostics - specific to this server implemention
     * @param diagnosticsList - data array of diagnostics data objects to fill
     * @return - true if completed successfully - does not indicate parse fail
     */
    bool parseDocumentForDiagnostics(
                          DataArray  & diagnosticsList );

    /** gather document completion items - specific to this server implemention
     * @param completionItems - data array of completion item objects to fill
     * @param is_incomplete - flag indicating if the completions are complete
     * @param line - line to be used for completions gathering logic
     * @param character - column to be used for completions gathering logic
     * @return - true if the gathering of items completed successfully
     */
    bool gatherDocumentCompletionItems(
                          DataArray & completionItems  ,
                          bool      & is_incomplete    ,
                          int         line             ,
                          int         character        );

    /** gather definition locations - specific to this server implemention
     * @param definitionLocations - data array of locations objects to fill
     * @param line - line to be used for locations gathering logic
     * @param character - column to be used for locations gathering logic
     * @return - true if the gathering of locations completed successfully
     */
    bool gatherDocumentDefinitionLocations(
                          DataArray & definitionLocations ,
                          int         line                ,
                          int         character           );

    /** get hover display text - logic specific to test server implemention
     * @param displayText - string reference to add text displayed on hover
     * @param line - zero-based line to use for finding node and hover text
     * @param character - zero-based column for finding node and hover text
     * @return - true if display text was added or unmodified without error
     */
    bool getHoverDisplayText(
                          std::string & displayText ,
                          int           line        ,
                          int           character   );

    /** gather references locations - specific to this server implemention
     * @param referencesLocations - data array of locations objects to fill
     * @param line - line to be used for locations gathering logic
     * @param character - column to be used for locations gathering logic
     * @param include_declaration - flag indicating declaration inclusion
     * @return - true if the gathering of locations completed successfully
     */
    bool gatherDocumentReferencesLocations(
                          DataArray & referencesLocations ,
                          int         line                ,
                          int         character           ,
                          bool        include_declaration );

    /** gather formatting text edits - specific to this server implemention
     * @param formattingTextEdits - data array of text edit objects to fill
     * @param tab_size - value of the size of a tab in spaces for formatting
     * @param insert_spaces - flag indicating whether to use spaces for tabs
     * @return - true if the gathering of text edits completed successfully
     */
    bool gatherDocumentFormattingTextEdits(
                          DataArray & formattingTextEdits ,
                          int         tab_size            ,
                          bool        insert_spaces       );

    /** gather document symbols - specific to this server implemention
     * @param documentSymbols - data array of symbols data objects to fill
     * @return - true if the gathering of symbols completed successfully
     */
    bool gatherDocumentSymbols(
                          DataArray & documentSymbols );

    /** gather extension responses - specific to this server implemention
     * @param extensionResponses - data array of custom responses to fill
     * @param extensionMethod - name for current extension request method
     * @param line - zero-based line to use for logic of custom extension
     * @param character - zero-based column for logic of custom extension
     * @return - true if request successfully handled with response built
     */
    bool gatherExtensionResponses(
                          wasp::DataArray   & extensionResponses ,
                          const std::string & extensionMethod    ,
                          int                 line               ,
                          int                 character          );

    /** read from connection into object - specific to this server's connection
     * @param object - reference to object to be read into
     * @return - true if the read from the connection completed successfully
     */
    bool connectionRead( DataObject & object )
    {
       return this->connection->read( object , this->errors );
    }

    /** write object json to connection - specific to this server's connection
     * @param object - reference to object with contents to write to connection
     * @return - true if the write to the connection completed successfully
     */
    bool connectionWrite( DataObject & object )
    {
       return this->connection->write( object , this->errors );
    }

    /**
     * @brief shared pointer to this server implementation's thread connection
     */
    std::shared_ptr<ThreadConnection> connection;
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_TEST_SERVER_H
