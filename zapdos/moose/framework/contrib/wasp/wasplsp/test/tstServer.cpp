#include "TestServer.h"
#include "wasplsp/Server.h"
#include "wasplsp/LSP.h"
#include "waspcore/Object.h"
#include "gtest/gtest.h"
#include <sstream>
#include <string>

namespace wasp {
namespace lsp  {

// handle for test server used for all of the tests

Server<TestServer> test_server;
int                test_request_id;

TEST(server, handle_initialize)
{
    // start test request id at one and increment by one after each request
    test_request_id = 1;

    DataObject        initializeRequest;
    std::stringstream errors;

    // initialize test parameters

    int               client_request_id =  test_request_id;
    int               client_process_id =  12345;
    std::string       client_root_path  = "test/root/uri/string";
    DataObject        client_capabilities;

    // build initialize request with the test parameters

    ASSERT_TRUE(buildInitializeRequest( initializeRequest   ,
                                        errors              ,
                                        client_request_id   ,
                                        client_process_id   ,
                                        client_root_path    ,
                                        client_capabilities ));

    DataObject initializeResponse;

    // handle the built initialize request with the test_server

    ASSERT_FALSE(test_server.clientSupportsSnippets());

    ASSERT_FALSE(test_server.clientSupportsWatchers());

    ASSERT_TRUE(test_server.handleInitializeRequest( initializeRequest  ,
                                                     initializeResponse ));
    test_request_id++;

    ASSERT_FALSE(test_server.clientSupportsSnippets());

    ASSERT_FALSE(test_server.clientSupportsWatchers());

    // check the formatted json from the test_server's response object

    std::stringstream json;
    initializeResponse.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "id" : 1
  ,"result" : {
    "capabilities" : {
    "completionProvider" : {
    "resolveProvider" : false
  }
    ,"definitionProvider" : true
    ,"documentFormattingProvider" : true
    ,"documentSymbolProvider" : true
    ,"extensionsProvider" : {
      "testMethod01" : true
      ,"testMethod02" : true
      ,"watcherRegistration" : true
    }
    ,"hoverProvider" : true
    ,"textDocumentSync" : {
      "change" : 1
      ,"openClose" : true
    }
  }
  }
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_initialized)
{
    DataObject        initializedNotification;
    std::stringstream errors;

    // build initialized notification

    ASSERT_TRUE(buildInitializedNotification( initializedNotification ,
                                              errors                  ));

    // handle the built initialized notification with the test_server

    ASSERT_TRUE(test_server.handleInitializedNotification( initializedNotification ));
}

TEST(server, handle_didopen)
{
    DataObject        didOpenNotification;
    std::stringstream errors;

    // didopen test parameters

    std::string       document_path        = "test/document/uri/string";
    std::string       document_language_id = "test_language_id_string";
    int               document_version     =  1;
    std::string       document_text        = "test\ntext\n1\nstring\n";

    // build didopen notification with the test parameters

    ASSERT_TRUE(buildDidOpenNotification( didOpenNotification  ,
                                          errors               ,
                                          document_path        ,
                                          document_language_id ,
                                          document_version     ,
                                          document_text        ));

    DataObject publishDiagnosticsNotification;

    // handle the built didopen notification with the test_server

    ASSERT_TRUE(test_server.handleDidOpenNotification( didOpenNotification ,
                                                       publishDiagnosticsNotification ));

    // check the formatted json from the test_server's diagnostics object

    std::stringstream json;
    publishDiagnosticsNotification.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "method" : "textDocument/publishDiagnostics"
  ,"params" : {
    "diagnostics" : [
    {
    "code" : "test.code.11"
    ,"message" : "Test message 11."
    ,"range" : {
      "end" : {
      "character" : 27
      ,"line" : 20
    }
      ,"start" : {
        "character" : 16
        ,"line" : 20
      }
    }
    ,"severity" : 1
    ,"source" : "test_source_11"
  }
    ,{
    "code" : "test.code.22"
    ,"message" : "Test message 22."
    ,"range" : {
      "end" : {
      "character" : 27
      ,"line" : 34
    }
      ,"start" : {
        "character" : 56
        ,"line" : 28
      }
    }
    ,"severity" : 2
    ,"source" : "test_source_22"
  }
    ,{
    "code" : "test.code.33"
    ,"message" : "Test message 33."
    ,"range" : {
      "end" : {
      "character" : 43
      ,"line" : 47
    }
      ,"start" : {
        "character" : 36
        ,"line" : 47
      }
    }
    ,"severity" : 3
    ,"source" : "test_source_33"
  }
  ]
    ,"uri" : "test/document/uri/string"
  }
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_didchange)
{
    DataObject        didChangeNotification;
    std::stringstream errors;

    // didchange test parameters

    std::string       document_path    = "test/document/uri/string";
    int               document_version =  2;
    int               start_line       =  0;
    int               start_character  =  0;
    int               end_line         =  3;
    int               end_character    =  7;
    int               range_length     =  0;
    std::string       text             = "test\ntext\n2\nstring\n";

    // build didchange notification with the test parameters

    ASSERT_TRUE(buildDidChangeNotification( didChangeNotification ,
                                            errors                ,
                                            document_path         ,
                                            document_version      ,
                                            start_line            ,
                                            start_character       ,
                                            end_line              ,
                                            end_character         ,
                                            range_length          ,
                                            text                  ));

    DataObject publishDiagnosticsNotification;

    // handle the built didchange notification with the test_server

    ASSERT_TRUE(test_server.handleDidChangeNotification( didChangeNotification          ,
                                                         publishDiagnosticsNotification ));

    // check the formatted json from the test_server's diagnostics object

    std::stringstream json;
    publishDiagnosticsNotification.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "method" : "textDocument/publishDiagnostics"
  ,"params" : {
    "diagnostics" : [
    {
    "code" : "test.code.44"
    ,"message" : "Test message 44."
    ,"range" : {
      "end" : {
      "character" : 16
      ,"line" : 68
    }
      ,"start" : {
        "character" : 45
        ,"line" : 67
      }
    }
    ,"severity" : 4
    ,"source" : "test_source_44"
  }
    ,{
    "code" : "test.code.55"
    ,"message" : "Test message 55."
    ,"range" : {
      "end" : {
      "character" : 12
      ,"line" : 88
    }
      ,"start" : {
        "character" : 17
        ,"line" : 87
      }
    }
    ,"severity" : 1
    ,"source" : "test_source_55"
  }
  ]
    ,"uri" : "test/document/uri/string"
  }
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_completion)
{
    DataObject        completionRequest;
    std::stringstream errors;

    // completion test parameters

    int               client_request_id =  test_request_id;
    std::string       document_path     = "test/document/uri/string";
    int               line              =  4;
    int               character         =  2;

    // build completion request with the test parameters

    ASSERT_TRUE(buildCompletionRequest( completionRequest  ,
                                        errors             ,
                                        client_request_id  ,
                                        document_path      ,
                                        line               ,
                                        character          ));

    DataObject completionResponse;

    // handle the built completion request with the test_server

    ASSERT_TRUE(test_server.handleCompletionRequest( completionRequest  ,
                                                     completionResponse ));
    test_request_id++;

    // check the formatted json from the test_server's response object

    std::stringstream json;
    completionResponse.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "id" : 2
  ,"result" : {
    "isIncomplete" : false
    ,"items" : [
      {
      "deprecated" : false
      ,"detail" : "test type info 1"
      ,"documentation" : "test documentation 1"
      ,"insertTextFormat" : 1
      ,"kind" : 23
      ,"label" : "test-label-1"
      ,"preselect" : true
      ,"textEdit" : {
        "newText" : "test-insert-text-1"
        ,"range" : {
          "end" : {
          "character" : 11
          ,"line" : 11
        }
          ,"start" : {
            "character" : 11
            ,"line" : 11
          }
        }
      }
    }
      ,{
      "deprecated" : false
      ,"detail" : "test type info 2"
      ,"documentation" : "test documentation 2"
      ,"insertTextFormat" : 2
      ,"kind" : 24
      ,"label" : "test-label-2"
      ,"preselect" : false
      ,"textEdit" : {
        "newText" : "test-insert-text-2"
        ,"range" : {
          "end" : {
          "character" : 22
          ,"line" : 22
        }
          ,"start" : {
            "character" : 22
            ,"line" : 22
          }
        }
      }
    }
      ,{
      "deprecated" : false
      ,"detail" : "test type info 3"
      ,"documentation" : "test documentation 3"
      ,"insertTextFormat" : 1
      ,"kind" : 25
      ,"label" : "test-label-3"
      ,"preselect" : false
      ,"textEdit" : {
        "newText" : "test-insert-text-3"
        ,"range" : {
          "end" : {
          "character" : 33
          ,"line" : 33
        }
          ,"start" : {
            "character" : 33
            ,"line" : 33
          }
        }
      }
    }
    ]
  }
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_definition)
{
    DataObject        definitionRequest;
    std::stringstream errors;

    // definition test parameters

    int               client_request_id =  test_request_id;
    std::string       document_path     = "test/document/uri/string";
    int               line              =  2;
    int               character         =  5;

    // build definition request with the test parameters

    ASSERT_TRUE(buildDefinitionRequest( definitionRequest  ,
                                        errors             ,
                                        client_request_id  ,
                                        document_path      ,
                                        line               ,
                                        character          ));

    DataObject definitionResponse;

    // handle the built definition request with the test_server

    ASSERT_TRUE(test_server.handleDefinitionRequest( definitionRequest  ,
                                                     definitionResponse ));
    test_request_id++;

    // check the formatted json from the test_server's response object

    std::stringstream json;
    definitionResponse.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "id" : 3
  ,"result" : [
    {
    "range" : {
    "end" : {
    "character" : 11
    ,"line" : 11
  }
    ,"start" : {
      "character" : 11
      ,"line" : 11
    }
  }
    ,"uri" : "test/document/uri/string"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 22
    ,"line" : 22
  }
    ,"start" : {
      "character" : 22
      ,"line" : 22
    }
  }
    ,"uri" : "test/document/uri/string"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 33
    ,"line" : 33
  }
    ,"start" : {
      "character" : 33
      ,"line" : 33
    }
  }
    ,"uri" : "test/document/uri/string"
  }
  ]
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_hover)
{
    // document hover test parameters using line and column for test server
    DataObject        hoverRequest;
    std::stringstream errors;
    int               client_request_id = test_request_id;
    std::string       document_path     = "test/document/uri/string";
    int               line              = 7;
    int               character         = 8;

    // build document hover request object using parameters for test server
    ASSERT_TRUE(buildHoverRequest( hoverRequest  ,
                                   errors             ,
                                   client_request_id  ,
                                   document_path      ,
                                   line               ,
                                   character          ));

    // handle document hover request object that was built with test server
    DataObject hoverResponse;
    ASSERT_TRUE(test_server.handleHoverRequest(hoverRequest, hoverResponse));
    test_request_id++;

    // check formatted json from test server document hover response object
    std::stringstream json;
    hoverResponse.format_json(json);
    std::stringstream json_expected;
    json_expected << R"INPUT({
  "id" : 4
  ,"result" : {
    "contents" : "this is the hover text from the test server"
  }
})INPUT";
    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_references)
{
    DataObject        referencesRequest;
    std::stringstream errors;

    // references test parameters

    int               client_request_id   =  test_request_id;
    std::string       document_path       = "test/document/uri/string";
    int               line                =  1;
    int               character           =  3;
    bool              include_declaration = false;

    // build references request with the test parameters

    ASSERT_TRUE(buildReferencesRequest( referencesRequest   ,
                                        errors              ,
                                        client_request_id   ,
                                        document_path       ,
                                        line                ,
                                        character           ,
                                        include_declaration ));

    DataObject referencesResponse;

    // handle the built references request with the test_server

    ASSERT_TRUE(test_server.handleReferencesRequest( referencesRequest  ,
                                                     referencesResponse ));
    test_request_id++;

    // check the formatted json from the test_server's response object

    std::stringstream json;
    referencesResponse.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "id" : 5
  ,"result" : [
    {
    "range" : {
    "end" : {
    "character" : 44
    ,"line" : 44
  }
    ,"start" : {
      "character" : 44
      ,"line" : 44
    }
  }
    ,"uri" : "test/document/uri/string"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 55
    ,"line" : 55
  }
    ,"start" : {
      "character" : 55
      ,"line" : 55
    }
  }
    ,"uri" : "test/document/uri/string"
  }
  ]
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_formatting)
{
    DataObject        formattingRequest;
    std::stringstream errors;

    // formatting test parameters

    int               client_request_id =  test_request_id;
    std::string       document_path     = "test/document/uri/string";
    int               tab_size          =  4;
    bool              insert_spaces     =  true;

    // build formatting request with the test parameters

    ASSERT_TRUE(buildFormattingRequest( formattingRequest ,
                                        errors            ,
                                        client_request_id ,
                                        document_path     ,
                                        tab_size          ,
                                        insert_spaces     ));

    DataObject formattingResponse;

    // handle the built formatting request with the test_server

    ASSERT_TRUE(test_server.handleFormattingRequest( formattingRequest  ,
                                                     formattingResponse ));
    test_request_id++;

    // check the formatted json from the test_server's response object

    std::stringstream json;
    formattingResponse.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "id" : 6
  ,"result" : [
    {
    "newText" : "test\n  new\n  text\n  format\n  1"
    ,"range" : {
      "end" : {
      "character" : 3
      ,"line" : 14
    }
      ,"start" : {
        "character" : 1
        ,"line" : 10
      }
    }
  }
    ,{
    "newText" : "test\n  new\n  text\n  format\n  2"
    ,"range" : {
      "end" : {
      "character" : 3
      ,"line" : 24
    }
      ,"start" : {
        "character" : 1
        ,"line" : 20
      }
    }
  }
    ,{
    "newText" : "test\n  new\n  text\n  format\n  3"
    ,"range" : {
      "end" : {
      "character" : 3
      ,"line" : 34
    }
      ,"start" : {
        "character" : 1
        ,"line" : 30
      }
    }
  }
  ]
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_symbols)
{
    DataObject        symbolsRequest;
    std::stringstream errors;

    // symbols test parameters

    int               client_request_id =  test_request_id;
    std::string       document_path     = "test/document/uri/string";

    // build symbols request with the test parameters

    ASSERT_TRUE(buildSymbolsRequest( symbolsRequest    ,
                                     errors            ,
                                     client_request_id ,
                                     document_path     ));

    DataObject symbolsResponse;

    // handle the built symbols request with the test_server

    ASSERT_TRUE(test_server.handleSymbolsRequest( symbolsRequest  ,
                                                  symbolsResponse ));
    test_request_id++;

    // check the formatted json from the test_server's response object

    std::stringstream json;
    symbolsResponse.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "id" : 7
  ,"result" : [
    {
    "children" : [
    {
    "children" : []
    ,"deprecated" : false
    ,"detail" : "test::symbol::detail::object::2"
    ,"kind" : 20
    ,"name" : "test_symbol_name_object_2"
    ,"range" : {
      "end" : {
      "character" : 27
      ,"line" : 20
    }
      ,"start" : {
        "character" : 21
        ,"line" : 20
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 25
      ,"line" : 20
    }
      ,"start" : {
        "character" : 23
        ,"line" : 20
      }
    }
  }
    ,{
    "children" : [
    {
    "children" : [
    {
    "children" : [
    {
    "children" : []
    ,"deprecated" : false
    ,"detail" : "test::symbol::detail::object::6"
    ,"kind" : 24
    ,"name" : "test_symbol_name_object_6"
    ,"range" : {
      "end" : {
      "character" : 67
      ,"line" : 60
    }
      ,"start" : {
        "character" : 61
        ,"line" : 60
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 65
      ,"line" : 60
    }
      ,"start" : {
        "character" : 63
        ,"line" : 60
      }
    }
  }
  ]
    ,"deprecated" : false
    ,"detail" : "test::symbol::detail::object::5"
    ,"kind" : 23
    ,"name" : "test_symbol_name_object_5"
    ,"range" : {
      "end" : {
      "character" : 57
      ,"line" : 50
    }
      ,"start" : {
        "character" : 51
        ,"line" : 50
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 55
      ,"line" : 50
    }
      ,"start" : {
        "character" : 53
        ,"line" : 50
      }
    }
  }
    ,{
    "children" : []
    ,"deprecated" : false
    ,"detail" : "test::symbol::detail::object::7"
    ,"kind" : 25
    ,"name" : "test_symbol_name_object_7"
    ,"range" : {
      "end" : {
      "character" : 77
      ,"line" : 70
    }
      ,"start" : {
        "character" : 71
        ,"line" : 70
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 75
      ,"line" : 70
    }
      ,"start" : {
        "character" : 73
        ,"line" : 70
      }
    }
  }
    ,{
    "children" : []
    ,"deprecated" : false
    ,"detail" : "test::symbol::detail::object::8"
    ,"kind" : 26
    ,"name" : "test_symbol_name_object_8"
    ,"range" : {
      "end" : {
      "character" : 87
      ,"line" : 80
    }
      ,"start" : {
        "character" : 81
        ,"line" : 80
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 85
      ,"line" : 80
    }
      ,"start" : {
        "character" : 83
        ,"line" : 80
      }
    }
  }
  ]
    ,"deprecated" : false
    ,"detail" : "test::symbol::detail::object::4"
    ,"kind" : 22
    ,"name" : "test_symbol_name_object_4"
    ,"range" : {
      "end" : {
      "character" : 47
      ,"line" : 40
    }
      ,"start" : {
        "character" : 41
        ,"line" : 40
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 45
      ,"line" : 40
    }
      ,"start" : {
        "character" : 43
        ,"line" : 40
      }
    }
  }
  ]
    ,"deprecated" : false
    ,"detail" : "test::symbol::detail::object::3"
    ,"kind" : 21
    ,"name" : "test_symbol_name_object_3"
    ,"range" : {
      "end" : {
      "character" : 37
      ,"line" : 30
    }
      ,"start" : {
        "character" : 31
        ,"line" : 30
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 35
      ,"line" : 30
    }
      ,"start" : {
        "character" : 33
        ,"line" : 30
      }
    }
  }
  ]
    ,"deprecated" : false
    ,"detail" : "test::symbol::detail::object::1"
    ,"kind" : 19
    ,"name" : "test_symbol_name_object_1"
    ,"range" : {
      "end" : {
      "character" : 17
      ,"line" : 10
    }
      ,"start" : {
        "character" : 11
        ,"line" : 10
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 15
      ,"line" : 10
    }
      ,"start" : {
        "character" : 13
        ,"line" : 10
      }
    }
  }
  ]
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_extension_01)
{
    DataObject        extensionRequest;
    std::stringstream errors;

    // extension 01 test parameters

    int         client_request_id =  test_request_id;
    std::string extension_method  = "testMethod01";
    std::string document_path     = "test/document/uri/string";
    int         line              =  3;
    int         character         =  4;

    // build extension 01 request with the test parameters

    ASSERT_TRUE(buildExtensionRequest( extensionRequest  ,
                                       errors            ,
                                       extension_method  ,
                                       client_request_id ,
                                       document_path     ,
                                       line              ,
                                       character         ));

    DataObject extensionResponse;

    // handle the built extension 01 request with the test_server

    ASSERT_TRUE(test_server.handleExtensionRequest( extension_method,
                                                    extensionRequest  ,
                                                    extensionResponse ));
    test_request_id++;

    // check the formatted json from the test_server's response object

    std::stringstream json;
    json.precision(15);
    extensionResponse.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "id" : 8
  ,"result" : [
    {
    "field_01" : "word_01"
    ,"field_02" : 111111111
    ,"field_03" : 1111.1111
  }
    ,{
    "field_01" : "word_02"
    ,"field_02" : 222222222
    ,"field_03" : 2222.2222
  }
    ,{
    "field_01" : "word_03"
    ,"field_02" : 333333333
    ,"field_03" : 3333.3333
  }
  ]
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_extension_02)
{
    DataObject        extensionRequest;
    std::stringstream errors;

    // extension 02 test parameters

    int         client_request_id =  test_request_id;
    std::string extension_method  = "testMethod02";
    std::string document_path     = "test/document/uri/string";
    int         line              =  6;
    int         character         =  7;

    // build extension 02 request with the test parameters

    ASSERT_TRUE(buildExtensionRequest( extensionRequest  ,
                                       errors            ,
                                       extension_method  ,
                                       client_request_id ,
                                       document_path     ,
                                       line              ,
                                       character         ));

    DataObject extensionResponse;

    // handle the built extension 02 request with the test_server

    ASSERT_TRUE(test_server.handleExtensionRequest( extension_method,
                                                    extensionRequest  ,
                                                    extensionResponse ));
    test_request_id++;

    // check the formatted json from the test_server's response object

    std::stringstream json;
    json.precision(15);
    extensionResponse.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "id" : 9
  ,"result" : [
    {
    "kind" : "lined"
    ,"name" : "title"
    ,"stdv" : [
      11.11
      ,22.22
      ,33.33
      ,44.44
    ]
    ,"xval" : [
      "aaa"
      ,"bbb"
      ,"ccc"
      ,"ddd"
    ]
    ,"yval" : [
      11111
      ,22222
      ,33333
      ,44444
    ]
  }
  ]
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_didclose)
{
    DataObject        didCloseNotification;
    std::stringstream errors;

    // didclose test parameter

    std::string  document_path = "test/document/uri/string";

    // build didclose notification with the test parameter

    ASSERT_TRUE(buildDidCloseNotification( didCloseNotification   ,
                                           errors                 ,
                                           document_path          ));

    // handle the built didclose notification with the test_server

    ASSERT_TRUE(test_server.handleDidCloseNotification( didCloseNotification ));
}

TEST(server, handle_shutdown)
{
    DataObject        shutdownRequest;
    std::stringstream errors;

    // shutdown test parameter

    int client_request_id =  test_request_id;

    // build shutdown request with the test parameters

    ASSERT_TRUE(buildShutdownRequest( shutdownRequest   ,
                                      errors            ,
                                      client_request_id ));

    DataObject shutdownResponse;

    // handle the built shutdown request with the test_server

    ASSERT_TRUE(test_server.handleShutdownRequest( shutdownRequest  ,
                                                   shutdownResponse ));
    test_request_id++;

    // check the formatted json from the test_server's response object

    std::stringstream json;
    shutdownResponse.format_json(json);

    std::stringstream json_expected;
    json_expected << R"INPUT({
  "id" : 10
  ,"result" : {}
})INPUT";

    ASSERT_EQ ( json_expected.str() , json.str() );
}

TEST(server, handle_exit)
{
    DataObject        exitNotification;
    std::stringstream errors;

    // build initialize notification with the test parameters

    ASSERT_TRUE(buildExitNotification( exitNotification ,
                                       errors           ));

    // handle the built exit notification with the test_server

    ASSERT_TRUE(test_server.handleExitNotification( exitNotification ));
}

} // namespace lsp
} // namespace wasp
