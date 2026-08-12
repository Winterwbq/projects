#include "TestServer.h"
#include "wasplsp/Server.h"
#include "wasplsp/ClientImpl.h"
#include "wasplsp/Client.h"
#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "wasplsp/SymbolIterator.h"
#include "waspcore/Object.h"
#include "gtest/gtest.h"
#include <thread>
#include <vector>
#include <sstream>
#include <string>
#include <set>
#include <map>

namespace wasp {
namespace lsp  {

// handles for client, test server, and server thread

Client<ClientImpl> test_client;
Server<TestServer> test_server;
std::thread        server_thread;
int                test_request_id;

TEST(client, launch_server_thread_and_connnect_client)
{
    // launch server thread and connect test client to server's communication

    server_thread = std::thread( &Server<TestServer>::run , &test_server );

    ASSERT_FALSE( test_client.isConnected() );

    // notify server about client extension capabilities in initialize call
    // set capabilities/extensions/testMethod01 = true
    // set capabilities/extensions/testMethod02 = true
    test_client.enableExtension("testMethod01");
    test_client.enableExtension("testMethod02");

    // notify server that client supports watchers by setting these to true
    // capabilities/workspace/didChangeWatchedFiles/dynamicRegistration
    // capabilities/workspace/didChangeWatchedFiles/relativePatternSupport
    test_client.enableWatcherSupport();

    ASSERT_TRUE ( test_client.connect( test_server.getConnection() ) );

    ASSERT_TRUE ( test_client.isConnected() );
}

TEST(client, initialize)
{
    // start test request id at one and increment by one after each request
    test_request_id = 1;

    // initialize

    ASSERT_FALSE( test_client.getConnection()->isServerRunning() );

    ASSERT_TRUE ( test_client.doInitialize() );

    test_request_id++;

    ASSERT_TRUE ( test_client.getConnection()->isServerRunning() );

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );
}

TEST(client, initialized)
{
    // initialized

    ASSERT_TRUE ( test_client.doInitialized() );

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );
}

TEST(client, document_open_and_diagnostics)
{
    // document open

    std::string document_path        = "test/document/uri/string";
    std::string document_language_id = "test-document-language-id";
    std::string document_text        = "test\ntext\n1\nstring\n";

    ASSERT_FALSE( test_client.isDocumentOpen() );

    ASSERT_TRUE ( test_client.doDocumentOpen( document_path        ,
                                              document_language_id ,
                                              document_text        ) );

    ASSERT_TRUE ( test_client.isDocumentOpen() );

    ASSERT_EQ   ( 1 , test_client.getCurrentDocumentVersion() );

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );

    // diagnostic responses

    ASSERT_EQ   ( 3 , test_client.getDiagnosticSize() );

    for (int index = 0; index < test_client.getDiagnosticSize(); index++)
    {
        clientDiagnostic diagnostic;

        ASSERT_TRUE ( test_client.getDiagnosticAt( index , diagnostic ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( 20                 , diagnostic.start_line      );
            ASSERT_EQ ( 16                 , diagnostic.start_character );
            ASSERT_EQ ( 20                 , diagnostic.end_line        );
            ASSERT_EQ ( 27                 , diagnostic.end_character   );
            ASSERT_EQ ( 1                  , diagnostic.severity        );
            ASSERT_EQ ( "test.code.11"     , diagnostic.code            );
            ASSERT_EQ ( "test_source_11"   , diagnostic.source          );
            ASSERT_EQ ( "Test message 11." , diagnostic.message         );
        }
        else if ( index == 1 )
        {
            ASSERT_EQ ( 28                 , diagnostic.start_line      );
            ASSERT_EQ ( 56                 , diagnostic.start_character );
            ASSERT_EQ ( 34                 , diagnostic.end_line        );
            ASSERT_EQ ( 27                 , diagnostic.end_character   );
            ASSERT_EQ ( 2                  , diagnostic.severity        );
            ASSERT_EQ ( "test.code.22"     , diagnostic.code            );
            ASSERT_EQ ( "test_source_22"   , diagnostic.source          );
            ASSERT_EQ ( "Test message 22." , diagnostic.message         );
        }
        else if ( index == 2 )
        {
            ASSERT_EQ ( 47                 , diagnostic.start_line      );
            ASSERT_EQ ( 36                 , diagnostic.start_character );
            ASSERT_EQ ( 47                 , diagnostic.end_line        );
            ASSERT_EQ ( 43                 , diagnostic.end_character   );
            ASSERT_EQ ( 3                  , diagnostic.severity        );
            ASSERT_EQ ( "test.code.33"     , diagnostic.code            );
            ASSERT_EQ ( "test_source_33"   , diagnostic.source          );
            ASSERT_EQ ( "Test message 33." , diagnostic.message         );
        }
    }
}

TEST(client, document_change_and_diagnostics)
{
    // document change

    int         start_line        = 0;
    int         start_character   = 0;
    int         end_line          = 3;
    int         end_character     = 7;
    int         range_length      = 0;
    std::string new_document_text = "test\ntext\n2\nstring\n";

    ASSERT_EQ   ( 1 , test_client.getCurrentDocumentVersion() );

    ASSERT_TRUE ( test_client.doDocumentChange( start_line        ,
                                                start_character   ,
                                                end_line          ,
                                                end_character     ,
                                                range_length      ,
                                                new_document_text ) );

    ASSERT_EQ   ( 2 , test_client.getCurrentDocumentVersion() );

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );

    // diagnostic responses

    ASSERT_EQ   ( 2 , test_client.getDiagnosticSize() );

    for (int index = 0; index < test_client.getDiagnosticSize(); index++)
    {
        clientDiagnostic diagnostic;

        ASSERT_TRUE ( test_client.getDiagnosticAt( index , diagnostic ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( 67                 , diagnostic.start_line      );
            ASSERT_EQ ( 45                 , diagnostic.start_character );
            ASSERT_EQ ( 68                 , diagnostic.end_line        );
            ASSERT_EQ ( 16                 , diagnostic.end_character   );
            ASSERT_EQ ( 4                  , diagnostic.severity        );
            ASSERT_EQ ( "test.code.44"     , diagnostic.code            );
            ASSERT_EQ ( "test_source_44"   , diagnostic.source          );
            ASSERT_EQ ( "Test message 44." , diagnostic.message         );
        }
        else if ( index == 1 )
        {
            ASSERT_EQ ( 87                 , diagnostic.start_line      );
            ASSERT_EQ ( 17                 , diagnostic.start_character );
            ASSERT_EQ ( 88                 , diagnostic.end_line        );
            ASSERT_EQ ( 12                 , diagnostic.end_character   );
            ASSERT_EQ ( 1                  , diagnostic.severity        );
            ASSERT_EQ ( "test.code.55"     , diagnostic.code            );
            ASSERT_EQ ( "test_source_55"   , diagnostic.source          );
            ASSERT_EQ ( "Test message 55." , diagnostic.message         );
        }
    }
}

TEST(client, document_completion_and_responses)
{
    // document completion

    int line        = 4;
    int character   = 2;

    ASSERT_TRUE ( test_client.doDocumentCompletion( line      ,
                                                    character ) );
    test_request_id++;

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );

    // completion responses

    ASSERT_EQ   ( 3 , test_client.getCompletionSize() );

    for (int index = 0; index < test_client.getCompletionSize(); index++)
    {
        clientCompletion completion;

        ASSERT_TRUE ( test_client.getCompletionAt( index , completion ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( "test-label-1"         , completion.label           );
            ASSERT_EQ ( 11                     , completion.start_line      );
            ASSERT_EQ ( 11                     , completion.start_character );
            ASSERT_EQ ( 11                     , completion.end_line        );
            ASSERT_EQ ( 11                     , completion.end_character   );
            ASSERT_EQ ( "test-insert-text-1"   , completion.new_text        );
            ASSERT_EQ ( m_comp_kind_event      , completion.kind            );
            ASSERT_EQ ( "test type info 1"     , completion.detail          );
            ASSERT_EQ ( "test documentation 1" , completion.documentation   );
            ASSERT_EQ ( false                  , completion.deprecated      );
            ASSERT_EQ ( true                   , completion.preselect       );
            ASSERT_EQ ( m_text_format_plaintext , completion.insert_text_format );
        }
        else if ( index == 1 )
        {
            ASSERT_EQ ( "test-label-2"         , completion.label           );
            ASSERT_EQ ( 22                     , completion.start_line      );
            ASSERT_EQ ( 22                     , completion.start_character );
            ASSERT_EQ ( 22                     , completion.end_line        );
            ASSERT_EQ ( 22                     , completion.end_character   );
            ASSERT_EQ ( "test-insert-text-2"   , completion.new_text        );
            ASSERT_EQ ( m_comp_kind_operator   , completion.kind            );
            ASSERT_EQ ( "test type info 2"     , completion.detail          );
            ASSERT_EQ ( "test documentation 2" , completion.documentation   );
            ASSERT_EQ ( false                  , completion.deprecated      );
            ASSERT_EQ ( false                  , completion.preselect       );
            ASSERT_EQ ( m_text_format_snippet  , completion.insert_text_format );
        }
        else if ( index == 2 )
        {
            ASSERT_EQ ( "test-label-3"         , completion.label           );
            ASSERT_EQ ( 33                     , completion.start_line      );
            ASSERT_EQ ( 33                     , completion.start_character );
            ASSERT_EQ ( 33                     , completion.end_line        );
            ASSERT_EQ ( 33                     , completion.end_character   );
            ASSERT_EQ ( "test-insert-text-3"   , completion.new_text        );
            ASSERT_EQ ( m_comp_kind_type_param , completion.kind            );
            ASSERT_EQ ( "test type info 3"     , completion.detail          );
            ASSERT_EQ ( "test documentation 3" , completion.documentation   );
            ASSERT_EQ ( false                  , completion.deprecated      );
            ASSERT_EQ ( false                  , completion.preselect       );
            ASSERT_EQ ( m_text_format_plaintext , completion.insert_text_format );
        }
    }
}

TEST(client, document_definition_and_responses)
{
    // document definition

    int line        = 2;
    int character   = 5;

    ASSERT_TRUE ( test_client.doDocumentDefinition( line      ,
                                                    character ) );
    test_request_id++;

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );

    // definition responses

    ASSERT_EQ   ( 3 , test_client.getDefinitionSize() );

    for (int index = 0; index < test_client.getDefinitionSize(); index++)
    {
        clientDefinition definition;

        ASSERT_TRUE ( test_client.getDefinitionAt( index , definition ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( 11 , definition.start_line      );
            ASSERT_EQ ( 11 , definition.start_character );
            ASSERT_EQ ( 11 , definition.end_line        );
            ASSERT_EQ ( 11 , definition.end_character   );
        }
        else if ( index == 1 )
        {
            ASSERT_EQ ( 22 , definition.start_line      );
            ASSERT_EQ ( 22 , definition.start_character );
            ASSERT_EQ ( 22 , definition.end_line        );
            ASSERT_EQ ( 22 , definition.end_character   );
        }
        else if ( index == 2 )
        {
            ASSERT_EQ ( 33 , definition.start_line      );
            ASSERT_EQ ( 33 , definition.start_character );
            ASSERT_EQ ( 33 , definition.end_line        );
            ASSERT_EQ ( 33 , definition.end_character   );
        }
    }
}

TEST(client, document_hover_and_response)
{
    // document hover test parameters using line and column for test server
    int line      = 7;
    int character = 8;

    // use client to do document hover with line and column for test server
    ASSERT_TRUE(test_client.doDocumentHover(line, character));
    test_request_id++;
    ASSERT_TRUE(test_client.getConnection()->getServerErrors().empty());
    ASSERT_TRUE(test_client.getErrors().empty());
    ASSERT_EQ(test_request_id - 1, test_client.getPreviousRequestID());

    // check contents of test server response to client document hover call
    std::string response_hover_text;
    ASSERT_TRUE(test_client.getHoverText(response_hover_text));
    ASSERT_TRUE(test_client.getErrors().empty());
    ASSERT_EQ("this is the hover text from the test server", response_hover_text);
}

TEST(client, document_references_and_responses)
{
    // document references

    int  line                = 1;
    int  character           = 3;
    bool include_declaration = false;

    ASSERT_TRUE ( test_client.doDocumentReferences( line                ,
                                                    character           ,
                                                    include_declaration ) );
    test_request_id++;

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );

    // references responses

    ASSERT_EQ   ( 2 , test_client.getReferencesSize() );

    for (int index = 0; index < test_client.getReferencesSize(); index++)
    {
        clientReference references;

        ASSERT_TRUE ( test_client.getReferencesAt( index , references ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( 44 , references.start_line      );
            ASSERT_EQ ( 44 , references.start_character );
            ASSERT_EQ ( 44 , references.end_line        );
            ASSERT_EQ ( 44 , references.end_character   );
        }
        else if ( index == 1 )
        {
            ASSERT_EQ ( 55 , references.start_line      );
            ASSERT_EQ ( 55 , references.start_character );
            ASSERT_EQ ( 55 , references.end_line        );
            ASSERT_EQ ( 55 , references.end_character   );
        }
    }
}

TEST(client, document_formatting_and_responses)
{
    // document formatting

    int         tab_size          =  4;
    bool        insert_spaces     =  true;

    ASSERT_TRUE ( test_client.doDocumentFormatting( tab_size      ,
                                                    insert_spaces ) );
    test_request_id++;

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );

    // formatting responses

    ASSERT_EQ   ( 3 , test_client.getFormattingSize() );

    for (int index = 0; index < test_client.getFormattingSize(); index++)
    {
        clientFormatting formatting;

        ASSERT_TRUE ( test_client.getFormattingAt( index , formatting ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( 10                                   , formatting.start_line      );
            ASSERT_EQ ( 01                                   , formatting.start_character );
            ASSERT_EQ ( 14                                   , formatting.end_line        );
            ASSERT_EQ ( 03                                   , formatting.end_character   );
            ASSERT_EQ ( "test\n  new\n  text\n  format\n  1" , formatting.new_text        );
        }
        else if ( index == 1 )
        {
            ASSERT_EQ ( 20                                   , formatting.start_line      );
            ASSERT_EQ ( 01                                   , formatting.start_character );
            ASSERT_EQ ( 24                                   , formatting.end_line        );
            ASSERT_EQ ( 03                                   , formatting.end_character   );
            ASSERT_EQ ( "test\n  new\n  text\n  format\n  2" , formatting.new_text        );
        }
        else if ( index == 2 )
        {
            ASSERT_EQ ( 30                                   , formatting.start_line      );
            ASSERT_EQ ( 01                                   , formatting.start_character );
            ASSERT_EQ ( 34                                   , formatting.end_line        );
            ASSERT_EQ ( 03                                   , formatting.end_character   );
            ASSERT_EQ ( "test\n  new\n  text\n  format\n  3" , formatting.new_text        );
        }
    }
}

TEST(client, document_symbols_and_responses)
{
    // document symbols

    ASSERT_TRUE ( test_client.doDocumentSymbols() );

    test_request_id++;

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );

    // symbols responses

    std::string expected_o1_name                      = "test_symbol_name_object_1";
    std::string expected_o1_detail                    = "test::symbol::detail::object::1";
    int         expected_o1_kind                      = m_symbol_kind_object;
    bool        expected_o1_deprecated                = false;
    int         expected_o1_start_line                = 10;
    int         expected_o1_start_character           = 11;
    int         expected_o1_end_line                  = 10;
    int         expected_o1_end_character             = 17;
    int         expected_o1_selection_start_line      = 10;
    int         expected_o1_selection_start_character = 13;
    int         expected_o1_selection_end_line        = 10;
    int         expected_o1_selection_end_character   = 15;

    std::string expected_o2_name                      = "test_symbol_name_object_2";
    std::string expected_o2_detail                    = "test::symbol::detail::object::2";
    int         expected_o2_kind                      = m_symbol_kind_key;
    bool        expected_o2_deprecated                = false;
    int         expected_o2_start_line                = 20;
    int         expected_o2_start_character           = 21;
    int         expected_o2_end_line                  = 20;
    int         expected_o2_end_character             = 27;
    int         expected_o2_selection_start_line      = 20;
    int         expected_o2_selection_start_character = 23;
    int         expected_o2_selection_end_line        = 20;
    int         expected_o2_selection_end_character   = 25;

    std::string expected_o3_name                      = "test_symbol_name_object_3";
    std::string expected_o3_detail                    = "test::symbol::detail::object::3";
    int         expected_o3_kind                      = m_symbol_kind_null;
    bool        expected_o3_deprecated                = false;
    int         expected_o3_start_line                = 30;
    int         expected_o3_start_character           = 31;
    int         expected_o3_end_line                  = 30;
    int         expected_o3_end_character             = 37;
    int         expected_o3_selection_start_line      = 30;
    int         expected_o3_selection_start_character = 33;
    int         expected_o3_selection_end_line        = 30;
    int         expected_o3_selection_end_character   = 35;

    std::string expected_o4_name                      = "test_symbol_name_object_4";
    std::string expected_o4_detail                    = "test::symbol::detail::object::4";
    int         expected_o4_kind                      = m_symbol_kind_enum_member;
    bool        expected_o4_deprecated                = false;
    int         expected_o4_start_line                = 40;
    int         expected_o4_start_character           = 41;
    int         expected_o4_end_line                  = 40;
    int         expected_o4_end_character             = 47;
    int         expected_o4_selection_start_line      = 40;
    int         expected_o4_selection_start_character = 43;
    int         expected_o4_selection_end_line        = 40;
    int         expected_o4_selection_end_character   = 45;

    std::string expected_o5_name                      = "test_symbol_name_object_5";
    std::string expected_o5_detail                    = "test::symbol::detail::object::5";
    int         expected_o5_kind                      = m_symbol_kind_struct;
    bool        expected_o5_deprecated                = false;
    int         expected_o5_start_line                = 50;
    int         expected_o5_start_character           = 51;
    int         expected_o5_end_line                  = 50;
    int         expected_o5_end_character             = 57;
    int         expected_o5_selection_start_line      = 50;
    int         expected_o5_selection_start_character = 53;
    int         expected_o5_selection_end_line        = 50;
    int         expected_o5_selection_end_character   = 55;

    std::string expected_o6_name                      = "test_symbol_name_object_6";
    std::string expected_o6_detail                    = "test::symbol::detail::object::6";
    int         expected_o6_kind                      = m_symbol_kind_event;
    bool        expected_o6_deprecated                = false;
    int         expected_o6_start_line                = 60;
    int         expected_o6_start_character           = 61;
    int         expected_o6_end_line                  = 60;
    int         expected_o6_end_character             = 67;
    int         expected_o6_selection_start_line      = 60;
    int         expected_o6_selection_start_character = 63;
    int         expected_o6_selection_end_line        = 60;
    int         expected_o6_selection_end_character   = 65;

    std::string expected_o7_name                      = "test_symbol_name_object_7";
    std::string expected_o7_detail                    = "test::symbol::detail::object::7";
    int         expected_o7_kind                      = m_symbol_kind_operator;
    bool        expected_o7_deprecated                = false;
    int         expected_o7_start_line                = 70;
    int         expected_o7_start_character           = 71;
    int         expected_o7_end_line                  = 70;
    int         expected_o7_end_character             = 77;
    int         expected_o7_selection_start_line      = 70;
    int         expected_o7_selection_start_character = 73;
    int         expected_o7_selection_end_line        = 70;
    int         expected_o7_selection_end_character   = 75;

    std::string expected_o8_name                      = "test_symbol_name_object_8";
    std::string expected_o8_detail                    = "test::symbol::detail::object::8";
    int         expected_o8_kind                      = m_symbol_kind_type_param;
    bool        expected_o8_deprecated                = false;
    int         expected_o8_start_line                = 80;
    int         expected_o8_start_character           = 81;
    int         expected_o8_end_line                  = 80;
    int         expected_o8_end_character             = 87;
    int         expected_o8_selection_start_line      = 80;
    int         expected_o8_selection_start_character = 83;
    int         expected_o8_selection_end_line        = 80;
    int         expected_o8_selection_end_character   = 85;

// - - - - - - - - -
// -               -
// -    root       -
// -     |         -
// -     o1        -
// -    /  \       -
// -  o2    o3     -
// -        |      -
// -        o4     -
// -     /  |  \   -
// -   o5  o7  o8  -
// -   |           -
// -   o6          -
// -               -
// - - - - - - - - -

    SymbolIterator::SP si = test_client.getSymbolIterator();

    int order = 0;

    std::stringstream paths;

    paths << std::endl;

    for( std::vector<int> indices{ 0 } ; indices.back() < (int) si->getChildSize() ; indices.back()++ )
    {
        si->moveToChildAt( indices.back() );

        indices.push_back( -1 );

        paths << si->getPath() << std::endl;

        std::string o_name;
        std::string o_detail;
        int         o_kind;
        bool        o_deprecated;
        int         o_start_line;
        int         o_start_character;
        int         o_end_line;
        int         o_end_character;
        int         o_selection_start_line;
        int         o_selection_start_character;
        int         o_selection_end_line;
        int         o_selection_end_character;

        ASSERT_TRUE( si->dissectCurrentSymbol( o_name                      ,
                                               o_detail                    ,
                                               o_kind                      ,
                                               o_deprecated                ,
                                               o_start_line                ,
                                               o_start_character           ,
                                               o_end_line                  ,
                                               o_end_character             ,
                                               o_selection_start_line      ,
                                               o_selection_start_character ,
                                               o_selection_end_line        ,
                                               o_selection_end_character   ));

        order++;

        if ( order == 1 )
        {
            ASSERT_EQ ( expected_o1_name                      , o_name                      );
            ASSERT_EQ ( expected_o1_detail                    , o_detail                    );
            ASSERT_EQ ( expected_o1_kind                      , o_kind                      );
            ASSERT_EQ ( expected_o1_deprecated                , o_deprecated                );
            ASSERT_EQ ( expected_o1_start_line                , o_start_line                );
            ASSERT_EQ ( expected_o1_start_character           , o_start_character           );
            ASSERT_EQ ( expected_o1_end_line                  , o_end_line                  );
            ASSERT_EQ ( expected_o1_end_character             , o_end_character             );
            ASSERT_EQ ( expected_o1_selection_start_line      , o_selection_start_line      );
            ASSERT_EQ ( expected_o1_selection_start_character , o_selection_start_character );
            ASSERT_EQ ( expected_o1_selection_end_line        , o_selection_end_line        );
            ASSERT_EQ ( expected_o1_selection_end_character   , o_selection_end_character   );
        }
        else if ( order == 2 )
        {
            ASSERT_EQ ( expected_o2_name                      , o_name                      );
            ASSERT_EQ ( expected_o2_detail                    , o_detail                    );
            ASSERT_EQ ( expected_o2_kind                      , o_kind                      );
            ASSERT_EQ ( expected_o2_deprecated                , o_deprecated                );
            ASSERT_EQ ( expected_o2_start_line                , o_start_line                );
            ASSERT_EQ ( expected_o2_start_character           , o_start_character           );
            ASSERT_EQ ( expected_o2_end_line                  , o_end_line                  );
            ASSERT_EQ ( expected_o2_end_character             , o_end_character             );
            ASSERT_EQ ( expected_o2_selection_start_line      , o_selection_start_line      );
            ASSERT_EQ ( expected_o2_selection_start_character , o_selection_start_character );
            ASSERT_EQ ( expected_o2_selection_end_line        , o_selection_end_line        );
            ASSERT_EQ ( expected_o2_selection_end_character   , o_selection_end_character   );
        }
        else if ( order == 3 )
        {
            ASSERT_EQ ( expected_o3_name                      , o_name                      );
            ASSERT_EQ ( expected_o3_detail                    , o_detail                    );
            ASSERT_EQ ( expected_o3_kind                      , o_kind                      );
            ASSERT_EQ ( expected_o3_deprecated                , o_deprecated                );
            ASSERT_EQ ( expected_o3_start_line                , o_start_line                );
            ASSERT_EQ ( expected_o3_start_character           , o_start_character           );
            ASSERT_EQ ( expected_o3_end_line                  , o_end_line                  );
            ASSERT_EQ ( expected_o3_end_character             , o_end_character             );
            ASSERT_EQ ( expected_o3_selection_start_line      , o_selection_start_line      );
            ASSERT_EQ ( expected_o3_selection_start_character , o_selection_start_character );
            ASSERT_EQ ( expected_o3_selection_end_line        , o_selection_end_line        );
            ASSERT_EQ ( expected_o3_selection_end_character   , o_selection_end_character   );
        }
        else if ( order == 4 )
        {
            ASSERT_EQ ( expected_o4_name                      , o_name                      );
            ASSERT_EQ ( expected_o4_detail                    , o_detail                    );
            ASSERT_EQ ( expected_o4_kind                      , o_kind                      );
            ASSERT_EQ ( expected_o4_deprecated                , o_deprecated                );
            ASSERT_EQ ( expected_o4_start_line                , o_start_line                );
            ASSERT_EQ ( expected_o4_start_character           , o_start_character           );
            ASSERT_EQ ( expected_o4_end_line                  , o_end_line                  );
            ASSERT_EQ ( expected_o4_end_character             , o_end_character             );
            ASSERT_EQ ( expected_o4_selection_start_line      , o_selection_start_line      );
            ASSERT_EQ ( expected_o4_selection_start_character , o_selection_start_character );
            ASSERT_EQ ( expected_o4_selection_end_line        , o_selection_end_line        );
            ASSERT_EQ ( expected_o4_selection_end_character   , o_selection_end_character   );
        }
        else if ( order == 5 )
        {
            ASSERT_EQ ( expected_o5_name                      , o_name                      );
            ASSERT_EQ ( expected_o5_detail                    , o_detail                    );
            ASSERT_EQ ( expected_o5_kind                      , o_kind                      );
            ASSERT_EQ ( expected_o5_deprecated                , o_deprecated                );
            ASSERT_EQ ( expected_o5_start_line                , o_start_line                );
            ASSERT_EQ ( expected_o5_start_character           , o_start_character           );
            ASSERT_EQ ( expected_o5_end_line                  , o_end_line                  );
            ASSERT_EQ ( expected_o5_end_character             , o_end_character             );
            ASSERT_EQ ( expected_o5_selection_start_line      , o_selection_start_line      );
            ASSERT_EQ ( expected_o5_selection_start_character , o_selection_start_character );
            ASSERT_EQ ( expected_o5_selection_end_line        , o_selection_end_line        );
            ASSERT_EQ ( expected_o5_selection_end_character   , o_selection_end_character   );
        }
        else if ( order == 6 )
        {
            ASSERT_EQ ( expected_o6_name                      , o_name                      );
            ASSERT_EQ ( expected_o6_detail                    , o_detail                    );
            ASSERT_EQ ( expected_o6_kind                      , o_kind                      );
            ASSERT_EQ ( expected_o6_deprecated                , o_deprecated                );
            ASSERT_EQ ( expected_o6_start_line                , o_start_line                );
            ASSERT_EQ ( expected_o6_start_character           , o_start_character           );
            ASSERT_EQ ( expected_o6_end_line                  , o_end_line                  );
            ASSERT_EQ ( expected_o6_end_character             , o_end_character             );
            ASSERT_EQ ( expected_o6_selection_start_line      , o_selection_start_line      );
            ASSERT_EQ ( expected_o6_selection_start_character , o_selection_start_character );
            ASSERT_EQ ( expected_o6_selection_end_line        , o_selection_end_line        );
            ASSERT_EQ ( expected_o6_selection_end_character   , o_selection_end_character   );
        }
        else if ( order == 7 )
        {
            ASSERT_EQ ( expected_o7_name                      , o_name                      );
            ASSERT_EQ ( expected_o7_detail                    , o_detail                    );
            ASSERT_EQ ( expected_o7_kind                      , o_kind                      );
            ASSERT_EQ ( expected_o7_deprecated                , o_deprecated                );
            ASSERT_EQ ( expected_o7_start_line                , o_start_line                );
            ASSERT_EQ ( expected_o7_start_character           , o_start_character           );
            ASSERT_EQ ( expected_o7_end_line                  , o_end_line                  );
            ASSERT_EQ ( expected_o7_end_character             , o_end_character             );
            ASSERT_EQ ( expected_o7_selection_start_line      , o_selection_start_line      );
            ASSERT_EQ ( expected_o7_selection_start_character , o_selection_start_character );
            ASSERT_EQ ( expected_o7_selection_end_line        , o_selection_end_line        );
            ASSERT_EQ ( expected_o7_selection_end_character   , o_selection_end_character   );
        }
        else if ( order == 8 )
        {
            ASSERT_EQ ( expected_o8_name                      , o_name                      );
            ASSERT_EQ ( expected_o8_detail                    , o_detail                    );
            ASSERT_EQ ( expected_o8_kind                      , o_kind                      );
            ASSERT_EQ ( expected_o8_deprecated                , o_deprecated                );
            ASSERT_EQ ( expected_o8_start_line                , o_start_line                );
            ASSERT_EQ ( expected_o8_start_character           , o_start_character           );
            ASSERT_EQ ( expected_o8_end_line                  , o_end_line                  );
            ASSERT_EQ ( expected_o8_end_character             , o_end_character             );
            ASSERT_EQ ( expected_o8_selection_start_line      , o_selection_start_line      );
            ASSERT_EQ ( expected_o8_selection_start_character , o_selection_start_character );
            ASSERT_EQ ( expected_o8_selection_end_line        , o_selection_end_line        );
            ASSERT_EQ ( expected_o8_selection_end_character   , o_selection_end_character   );
        }

        if ( si->getChildSize() == 0 )
        {
            while ( indices.back()+1 == (int) si->getChildSize() && si->moveToParent() )
            {
                indices.pop_back();
            }
        }
    }

    std::stringstream expected_paths;
    expected_paths << R"INPUT(
/test_symbol_name_object_1 (10:11)
/test_symbol_name_object_1/test_symbol_name_object_2 (20:21)
/test_symbol_name_object_1/test_symbol_name_object_3 (30:31)
/test_symbol_name_object_1/test_symbol_name_object_3/test_symbol_name_object_4 (40:41)
/test_symbol_name_object_1/test_symbol_name_object_3/test_symbol_name_object_4/test_symbol_name_object_5 (50:51)
/test_symbol_name_object_1/test_symbol_name_object_3/test_symbol_name_object_4/test_symbol_name_object_5/test_symbol_name_object_6 (60:61)
/test_symbol_name_object_1/test_symbol_name_object_3/test_symbol_name_object_4/test_symbol_name_object_7 (70:71)
/test_symbol_name_object_1/test_symbol_name_object_3/test_symbol_name_object_4/test_symbol_name_object_8 (80:81)
)INPUT";

    ASSERT_EQ ( expected_paths.str() , paths.str() );
}

TEST(client, workspace_handle_watch_files)
{
    // verify client and server support watch files and handle registration
    ASSERT_TRUE(test_client.hasWatcherSupport());
    ASSERT_TRUE(test_client.serverSupportsExtension("watcherRegistration"));
    ASSERT_TRUE(test_client.handleWatchFileRegistration());

    // get watch files that were registered by server and verify file paths
    std::set<std::string> watch_uris = test_client.getAllWatchFiles();
    ASSERT_EQ((std::size_t)2, watch_uris.size());
    std::size_t watch_uri_index = 0;
    for (const std::string & watch_uri : watch_uris)
    {
        if (watch_uri_index == 0)
            ASSERT_EQ("/path/to/resource/file01.i", removeUriScheme(watch_uri));
        else if (watch_uri_index == 1)
            ASSERT_EQ("/path/to/resource/file02.i", removeUriScheme(watch_uri));
        watch_uri_index++;
    }

    // notify server watch files have changed and get base file diagnostics
    std::map<std::string, std::vector<clientDiagnostic>> all_diagnostics;
    ASSERT_TRUE(test_client.notifyServerChangedWatchedFiles(watch_uris, all_diagnostics));
    ASSERT_EQ((std::size_t)1, all_diagnostics.size());

    // verify base file path and expected diagnostics from server reprocess
    std::string base_uri                           = all_diagnostics.begin()->first;
    std::vector<clientDiagnostic> base_diagnostics = all_diagnostics.begin()->second;
    ASSERT_EQ("test/document/uri/string", removeUriScheme(base_uri));
    ASSERT_EQ((std::size_t)2, base_diagnostics.size());
    std::size_t diagnostic_index = 0;
    for (const clientDiagnostic & diagnostic : base_diagnostics)
    {
        if (diagnostic_index == 0)
        {
            ASSERT_EQ(67                , diagnostic.start_line     );
            ASSERT_EQ(45                , diagnostic.start_character);
            ASSERT_EQ(68                , diagnostic.end_line       );
            ASSERT_EQ(16                , diagnostic.end_character  );
            ASSERT_EQ(4                 , diagnostic.severity       );
            ASSERT_EQ("test.code.44"    , diagnostic.code           );
            ASSERT_EQ("test_source_44"  , diagnostic.source         );
            ASSERT_EQ("Test message 44.", diagnostic.message        );
        }
        else if (diagnostic_index == 1)
        {
            ASSERT_EQ(87                , diagnostic.start_line     );
            ASSERT_EQ(17                , diagnostic.start_character);
            ASSERT_EQ(88                , diagnostic.end_line       );
            ASSERT_EQ(12                , diagnostic.end_character  );
            ASSERT_EQ(1                 , diagnostic.severity       );
            ASSERT_EQ("test.code.55"    , diagnostic.code           );
            ASSERT_EQ("test_source_55"  , diagnostic.source         );
            ASSERT_EQ("Test message 55.", diagnostic.message        );
        }
        diagnostic_index++;
    }
}

TEST(client, extension_request_and_response_01)
{
    // extension 01 request

    std::string extension_method  = "testMethod01";
    int         line              =  3;
    int         character         =  4;

    ASSERT_TRUE ( test_client.doExtensionMethod( extension_method ,
                                                 line             ,
                                                 character        ) );
    test_request_id++;

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );

    // extension 01 response

    ASSERT_EQ   ( 3 , test_client.getExtensionResponseSize() );

    for (int index = 0; index < test_client.getExtensionResponseSize(); index++)
    {
        DataObject extension_response;

        ASSERT_TRUE ( test_client.getExtensionResponseAt( index , extension_response ) );

        if ( index == 0 )
        {
            ASSERT_EQ ( (std::size_t) 3 , extension_response.size());

            ASSERT_TRUE ( extension_response["field_01"].is_string() );
            ASSERT_TRUE ( extension_response["field_02"].is_int()    );
            ASSERT_TRUE ( extension_response["field_03"].is_double() );

            ASSERT_EQ ( "word_01" , extension_response["field_01"].to_string() );
            ASSERT_EQ ( 111111111 , extension_response["field_02"].to_int()    );
            ASSERT_EQ ( 1111.1111 , extension_response["field_03"].to_double() );
        }
        else if ( index == 1 )
        {
            ASSERT_EQ ( (std::size_t) 3 , extension_response.size());

            ASSERT_TRUE ( extension_response["field_01"].is_string() );
            ASSERT_TRUE ( extension_response["field_02"].is_int()    );
            ASSERT_TRUE ( extension_response["field_03"].is_double() );

            ASSERT_EQ ( "word_02" , extension_response["field_01"].to_string() );
            ASSERT_EQ ( 222222222 , extension_response["field_02"].to_int()    );
            ASSERT_EQ ( 2222.2222 , extension_response["field_03"].to_double() );
        }
        else if ( index == 2 )
        {
            ASSERT_EQ ( (std::size_t) 3 , extension_response.size());

            ASSERT_TRUE ( extension_response["field_01"].is_string() );
            ASSERT_TRUE ( extension_response["field_02"].is_int()    );
            ASSERT_TRUE ( extension_response["field_03"].is_double() );

            ASSERT_EQ ( "word_03" , extension_response["field_01"].to_string() );
            ASSERT_EQ ( 333333333 , extension_response["field_02"].to_int()    );
            ASSERT_EQ ( 3333.3333 , extension_response["field_03"].to_double() );
        }
    }
}

TEST(client, extension_request_and_response_02)
{
    // extension 02 request

    std::string extension_method  = "testMethod02";
    int         line              =  6;
    int         character         =  7;

    ASSERT_TRUE ( test_client.doExtensionMethod( extension_method ,
                                                 line             ,
                                                 character        ) );
    test_request_id++;

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );

    // extension 02 response

    ASSERT_EQ   ( 1 , test_client.getExtensionResponseSize() );

    DataObject extension_response;

    ASSERT_TRUE ( test_client.getExtensionResponseAt( 0 , extension_response ) );

    ASSERT_EQ ( (std::size_t) 5 , extension_response.size());

    ASSERT_TRUE ( extension_response["name"].is_string() );
    ASSERT_TRUE ( extension_response["kind"].is_string() );
    ASSERT_TRUE ( extension_response["xval"].is_array()  );
    ASSERT_TRUE ( extension_response["yval"].is_array()  );
    ASSERT_TRUE ( extension_response["stdv"].is_array()  );

    DataArray* xval = extension_response["xval"].to_array();
    DataArray* yval = extension_response["yval"].to_array();
    DataArray* stdv = extension_response["stdv"].to_array();

    ASSERT_EQ ( (std::size_t) 4 , xval->size() );
    ASSERT_EQ ( (std::size_t) 4 , yval->size() );
    ASSERT_EQ ( (std::size_t) 4 , stdv->size() );

    ASSERT_EQ ( "title" , extension_response["name"].to_string() );
    ASSERT_EQ ( "lined" , extension_response["kind"].to_string() );

    ASSERT_EQ ( "aaa" , xval->at(0).to_string() );
    ASSERT_EQ ( "bbb" , xval->at(1).to_string() );
    ASSERT_EQ ( "ccc" , xval->at(2).to_string() );
    ASSERT_EQ ( "ddd" , xval->at(3).to_string() );

    ASSERT_EQ ( 11111 , yval->at(0).to_int() );
    ASSERT_EQ ( 22222 , yval->at(1).to_int() );
    ASSERT_EQ ( 33333 , yval->at(2).to_int() );
    ASSERT_EQ ( 44444 , yval->at(3).to_int() );

    ASSERT_EQ ( 11.11 , stdv->at(0).to_double() );
    ASSERT_EQ ( 22.22 , stdv->at(1).to_double() );
    ASSERT_EQ ( 33.33 , stdv->at(2).to_double() );
    ASSERT_EQ ( 44.44 , stdv->at(3).to_double() );
}

TEST(client, document_close)
{
    // document close

    ASSERT_TRUE ( test_client.isDocumentOpen() );

    ASSERT_TRUE ( test_client.doDocumentClose() );

    ASSERT_FALSE( test_client.isDocumentOpen() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );
}

TEST(client, shutdown)
{
    // shutdown

    ASSERT_TRUE ( test_client.getConnection()->isServerRunning() );

    ASSERT_TRUE ( test_client.doShutdown() );

    test_request_id++;

    ASSERT_FALSE( test_client.getConnection()->isServerRunning() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );
}

TEST(client, exit)
{
    // exit

    ASSERT_TRUE ( test_client.doExit() );

    ASSERT_TRUE ( test_client.getErrors().empty() );

    ASSERT_TRUE ( test_client.getConnection()->getServerErrors().empty() );

    ASSERT_EQ   ( test_request_id - 1 , test_client.getPreviousRequestID() );
}

TEST(client, server_thread_join)
{
    // make sure server thread finishes execution from the exit notification

    server_thread.join();
}

} // namespace lsp
} // namespace wasp
