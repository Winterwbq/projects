import unittest
from wasp import *
from .example_server import ExampleServer

class TestLspServerMethods(unittest.TestCase):

    def test_server_methods(self):
        error_stream = stringstream()
        example_server = ExampleServer()

        with self.subTest(msg='server_methods.initialize'):
            # Build test initialize request and use server to handle
            initialize_request  = DataObject()
            client_request_id   = 1
            client_process_id   = -1
            client_root_path    = ""
            client_capabilities = DataObject()
            self.assertTrue(buildInitializeRequest(initialize_request,
                                                   error_stream,
                                                   client_request_id,
                                                   client_process_id,
                                                   client_root_path,
                                                   client_capabilities))
            self.assertFalse(error_stream.str())
            initialize_response = DataObject()
            self.assertFalse(example_server.isRunning())
            self.assertTrue(example_server.handleInitializeRequest(initialize_request,
                                                                   initialize_response))
            self.assertTrue(example_server.isRunning())
            self.assertFalse(example_server.clientSupportsSnippets())
            self.assertFalse(example_server.getErrors())
            # Check body of json rpc from server initialize response
            json_actual = stringstream()
            json_expect = '''
{
  "id" : 1
  ,"result" : {
    "capabilities" : {
    "completionProvider" : {
    "resolveProvider" : false
  }
    ,"definitionProvider" : true
    ,"documentFormattingProvider" : true
    ,"documentSymbolProvider" : true
    ,"hoverProvider" : true
    ,"referencesProvider" : true
    ,"textDocumentSync" : {
      "change" : 1
      ,"openClose" : true
    }
  }
  }
}
            '''
            self.assertTrue(initialize_response.format_json(json_actual))
            self.assertEqual(json_expect.strip(), json_actual.str())
            # Check values dissected from server initialize response
            server_capabilities = DataObject()
            success, server_response_id = dissectInitializeResponse(initialize_response,
                                                                    error_stream,
                                                                    server_capabilities)
            self.assertTrue(success)
            self.assertFalse(error_stream.str())
            self.assertEqual(client_request_id, server_response_id)
            self.assertEqual(7, server_capabilities.size())
            # Build initialized notification and handle using server
            initialized_notification = DataObject()
            self.assertTrue(buildInitializedNotification(initialized_notification, error_stream))
            self.assertFalse(error_stream.str())
            self.assertTrue(example_server.handleInitializedNotification(initialized_notification))
            self.assertFalse(example_server.getErrors())

        with self.subTest(msg='server_methods.didopen'):
            # Build test didopen notification and handle with server
            didopen_notification = DataObject()
            document_path        = "test/path/to/doc"
            document_language_id = "test_language_id"
            document_text        = "test doc text 01"
            document_version     = 1
            self.assertTrue(buildDidOpenNotification(didopen_notification,
                                                     error_stream,
                                                     document_path,
                                                     document_language_id,
                                                     document_version,
                                                     document_text))
            self.assertFalse(error_stream.str())
            diag_notification = DataObject()
            self.assertTrue(example_server.handleDidOpenNotification(didopen_notification,
                                                                     diag_notification))
            self.assertFalse(example_server.getErrors())
            # Check json rpc body of server diagnostics notification
            json_actual = stringstream()
            json_expect = '''
{
  "method" : "textDocument/publishDiagnostics"
  ,"params" : {
    "diagnostics" : [
    {
    "code" : "code.101"
    ,"message" : "message 101"
    ,"range" : {
      "end" : {
      "character" : 14
      ,"line" : 13
    }
      ,"start" : {
        "character" : 12
        ,"line" : 11
      }
    }
    ,"severity" : 1
    ,"source" : "source_101"
  }
    ,{
    "code" : "code.202"
    ,"message" : "message 202"
    ,"range" : {
      "end" : {
      "character" : 24
      ,"line" : 23
    }
      ,"start" : {
        "character" : 22
        ,"line" : 21
      }
    }
    ,"severity" : 2
    ,"source" : "source_202"
  }
    ,{
    "code" : "code.303"
    ,"message" : "message 303"
    ,"range" : {
      "end" : {
      "character" : 34
      ,"line" : 33
    }
      ,"start" : {
        "character" : 32
        ,"line" : 31
      }
    }
    ,"severity" : 3
    ,"source" : "source_303"
  }
  ]
    ,"uri" : "test/path/to/doc"
  }
}
            '''
            self.assertTrue(diag_notification.format_json(json_actual))
            self.assertEqual(json_expect.strip(), json_actual.str())
            # Check server diagnostics notification values dissected
            response_path, diag_array = string(), DataArray()
            self.assertTrue(dissectPublishDiagnosticsNotification(diag_notification,
                                                                  error_stream,
                                                                  response_path,
                                                                  diag_array))
            self.assertFalse(error_stream.str())
            self.assertEqual(document_path, response_path)
            self.assertEqual(3, diag_array.size())
            for i in range(diag_array.size()):
                diag_object = diag_array.at(i).to_object()
                code, src, msg = string(), string(), string()
                success, beg_line, beg_char, end_line, end_char, severity = \
                    dissectDiagnosticObject(diag_object, error_stream, code, src, msg)
                self.assertTrue(success)
                if i == 0:
                    self.assertEqual( 11            , beg_line )
                    self.assertEqual( 12            , beg_char )
                    self.assertEqual( 13            , end_line )
                    self.assertEqual( 14            , end_char )
                    self.assertEqual( 1             , severity )
                    self.assertEqual( "code.101"    , code     )
                    self.assertEqual( "source_101"  , src      )
                    self.assertEqual( "message 101" , msg      )
                elif i == 1:
                    self.assertEqual( 21            , beg_line )
                    self.assertEqual( 22            , beg_char )
                    self.assertEqual( 23            , end_line )
                    self.assertEqual( 24            , end_char )
                    self.assertEqual( 2             , severity )
                    self.assertEqual( "code.202"    , code     )
                    self.assertEqual( "source_202"  , src      )
                    self.assertEqual( "message 202" , msg      )
                elif i == 2:
                    self.assertEqual( 31            , beg_line )
                    self.assertEqual( 32            , beg_char )
                    self.assertEqual( 33            , end_line )
                    self.assertEqual( 34            , end_char )
                    self.assertEqual( 3             , severity )
                    self.assertEqual( "code.303"    , code     )
                    self.assertEqual( "source_303"  , src      )
                    self.assertEqual( "message 303" , msg      )

        with self.subTest(msg='server_methods.didchange'):
            # Build test didchange notification and handle in server
            didchange_notification = DataObject()
            document_path          = "test/path/to/doc"
            document_text_change   = "test doc text 02"
            document_version       = 2
            self.assertTrue(buildDidChangeNotification(didchange_notification,
                                                       error_stream,
                                                       document_path,
                                                       document_version,
                                                       -1, -1, -1, -1, -1,
                                                       document_text_change))
            self.assertFalse(error_stream.str())
            diag_notification = DataObject()
            self.assertTrue(example_server.handleDidChangeNotification(didchange_notification,
                                                                       diag_notification))
            self.assertFalse(example_server.getErrors())
            # Check json rpc body of server diagnostics notification
            json_actual = stringstream()
            json_expect = '''
{
  "method" : "textDocument/publishDiagnostics"
  ,"params" : {
    "diagnostics" : [
    {
    "code" : "code.404"
    ,"message" : "message 404"
    ,"range" : {
      "end" : {
      "character" : 44
      ,"line" : 43
    }
      ,"start" : {
        "character" : 42
        ,"line" : 41
      }
    }
    ,"severity" : 1
    ,"source" : "source_404"
  }
    ,{
    "code" : "code.505"
    ,"message" : "message 505"
    ,"range" : {
      "end" : {
      "character" : 54
      ,"line" : 53
    }
      ,"start" : {
        "character" : 52
        ,"line" : 51
      }
    }
    ,"severity" : 2
    ,"source" : "source_505"
  }
  ]
    ,"uri" : "test/path/to/doc"
  }
}
            '''
            self.assertTrue(diag_notification.format_json(json_actual))
            self.assertEqual(json_expect.strip(), json_actual.str())
            # Check server diagnostics notification values dissected
            response_path, diag_array = string(), DataArray()
            self.assertTrue(dissectPublishDiagnosticsNotification(diag_notification,
                                                                  error_stream,
                                                                  response_path,
                                                                  diag_array))
            self.assertFalse(error_stream.str())
            self.assertEqual(document_path, response_path)
            self.assertEqual(2, diag_array.size())
            for i in range(diag_array.size()):
                diag_object = diag_array.at(i).to_object()
                code, src, msg = string(), string(), string()
                success, beg_line, beg_char, end_line, end_char, severity = \
                    dissectDiagnosticObject(diag_object, error_stream, code, src, msg)
                self.assertTrue(success)
                if i == 0:
                    self.assertEqual( 41            , beg_line )
                    self.assertEqual( 42            , beg_char )
                    self.assertEqual( 43            , end_line )
                    self.assertEqual( 44            , end_char )
                    self.assertEqual( 1             , severity )
                    self.assertEqual( "code.404"    , code     )
                    self.assertEqual( "source_404"  , src      )
                    self.assertEqual( "message 404" , msg      )
                elif i == 1:
                    self.assertEqual( 51            , beg_line )
                    self.assertEqual( 52            , beg_char )
                    self.assertEqual( 53            , end_line )
                    self.assertEqual( 54            , end_char )
                    self.assertEqual( 2             , severity )
                    self.assertEqual( "code.505"    , code     )
                    self.assertEqual( "source_505"  , src      )
                    self.assertEqual( "message 505" , msg      )

        with self.subTest(msg='server_methods.symbols'):
            # Build test documentsymbols request to handle in server
            symbols_request   = DataObject()
            document_path     = "test/path/to/doc"
            client_request_id = 2
            self.assertTrue(buildSymbolsRequest(symbols_request,
                                                error_stream,
                                                client_request_id,
                                                document_path))
            self.assertFalse(error_stream.str())
            symbols_response = DataObject()
            self.assertTrue(example_server.handleSymbolsRequest(symbols_request,
                                                                symbols_response))
            self.assertFalse(example_server.getErrors())
            # Check json rpc body of server documentsymbols response
            json_actual = stringstream()
            json_expect = '''
{
  "id" : 2
  ,"result" : [
    {
    "children" : [
    {
    "children" : []
    ,"deprecated" : false
    ,"detail" : "child.01.detail"
    ,"kind" : 23
    ,"name" : "child_01_name"
    ,"range" : {
      "end" : {
      "character" : 32
      ,"line" : 31
    }
      ,"start" : {
        "character" : 12
        ,"line" : 11
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 32
      ,"line" : 31
    }
      ,"start" : {
        "character" : 12
        ,"line" : 11
      }
    }
  }
    ,{
    "children" : []
    ,"deprecated" : false
    ,"detail" : "child.02.detail"
    ,"kind" : 6
    ,"name" : "child_02_name"
    ,"range" : {
      "end" : {
      "character" : 62
      ,"line" : 61
    }
      ,"start" : {
        "character" : 42
        ,"line" : 41
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 62
      ,"line" : 61
    }
      ,"start" : {
        "character" : 42
        ,"line" : 41
      }
    }
  }
  ]
    ,"deprecated" : false
    ,"detail" : "doc.root.detail"
    ,"kind" : 19
    ,"name" : "doc_root_name"
    ,"range" : {
      "end" : {
      "character" : 62
      ,"line" : 61
    }
      ,"start" : {
        "character" : 12
        ,"line" : 11
      }
    }
    ,"selectionRange" : {
      "end" : {
      "character" : 62
      ,"line" : 61
    }
      ,"start" : {
        "character" : 12
        ,"line" : 11
      }
    }
  }
  ]
}
            '''
            self.assertTrue(symbols_response.format_json(json_actual))
            self.assertEqual(json_expect.strip(), json_actual.str())
            # Check server documentsymbols response values dissected
            symbols_array = DataArray()
            success, server_response_id = dissectSymbolsResponse(symbols_response,
                                                                 error_stream,
                                                                 symbols_array)
            self.assertTrue(success)
            self.assertFalse(error_stream.str())
            self.assertEqual(client_request_id, server_response_id)
            self.assertEqual(1, symbols_array.size())
            symbol_doc_root = symbols_array.at(0).to_object()
            self.assertEqual(2, getDocumentSymbolChildren(symbol_doc_root).size())
            symbol_child_01 = getDocumentSymbolChildren(symbol_doc_root).at(0).to_object()
            symbol_child_02 = getDocumentSymbolChildren(symbol_doc_root).at(1).to_object()
            self.assertEqual(0, getDocumentSymbolChildren(symbol_child_01).size())
            self.assertEqual(0, getDocumentSymbolChildren(symbol_child_02).size())

            name, detail = string(), string()
            success,      kind,         deprecated,                  \
            beg_line,     beg_char,     end_line,     end_char,      \
            beg_line_sel, beg_char_sel, end_line_sel, end_char_sel = \
                dissectDocumentSymbolObject(symbol_doc_root, error_stream, name, detail)
            self.assertTrue(success)
            self.assertEqual( "doc_root_name"      , name         )
            self.assertEqual( "doc.root.detail"    , detail       )
            self.assertEqual( m_symbol_kind_object , kind         )
            self.assertEqual( False                , deprecated   )
            self.assertEqual( 11                   , beg_line     )
            self.assertEqual( 12                   , beg_char     )
            self.assertEqual( 61                   , end_line     )
            self.assertEqual( 62                   , end_char     )
            self.assertEqual( 11                   , beg_line_sel )
            self.assertEqual( 12                   , beg_char_sel )
            self.assertEqual( 61                   , end_line_sel )
            self.assertEqual( 62                   , end_char_sel )

            name, detail = string(), string()
            success,      kind,         deprecated,                  \
            beg_line,     beg_char,     end_line,     end_char,      \
            beg_line_sel, beg_char_sel, end_line_sel, end_char_sel = \
                dissectDocumentSymbolObject(symbol_child_01, error_stream, name, detail)
            self.assertTrue(success)
            self.assertEqual( "child_01_name"      , name         )
            self.assertEqual( "child.01.detail"    , detail       )
            self.assertEqual( m_symbol_kind_struct , kind         )
            self.assertEqual( False                , deprecated   )
            self.assertEqual( 11                   , beg_line     )
            self.assertEqual( 12                   , beg_char     )
            self.assertEqual( 31                   , end_line     )
            self.assertEqual( 32                   , end_char     )
            self.assertEqual( 11                   , beg_line_sel )
            self.assertEqual( 12                   , beg_char_sel )
            self.assertEqual( 31                   , end_line_sel )
            self.assertEqual( 32                   , end_char_sel )

            name, detail = string(), string()
            success,      kind,         deprecated,                  \
            beg_line,     beg_char,     end_line,     end_char,      \
            beg_line_sel, beg_char_sel, end_line_sel, end_char_sel = \
                dissectDocumentSymbolObject(symbol_child_02, error_stream, name, detail)
            self.assertTrue(success)
            self.assertEqual( "child_02_name"      , name         )
            self.assertEqual( "child.02.detail"    , detail       )
            self.assertEqual( m_symbol_kind_method , kind         )
            self.assertEqual( False                , deprecated   )
            self.assertEqual( 41                   , beg_line     )
            self.assertEqual( 42                   , beg_char     )
            self.assertEqual( 61                   , end_line     )
            self.assertEqual( 62                   , end_char     )
            self.assertEqual( 41                   , beg_line_sel )
            self.assertEqual( 42                   , beg_char_sel )
            self.assertEqual( 61                   , end_line_sel )
            self.assertEqual( 62                   , end_char_sel )

        with self.subTest(msg='server_methods.completion'):
            # Build test completion request and use server to handle
            completion_request         = DataObject()
            document_path              = "test/path/to/doc"
            client_request_id          = 3
            request_line, request_char = 23, 45
            self.assertTrue(buildCompletionRequest(completion_request,
                                                   error_stream,
                                                   client_request_id,
                                                   document_path,
                                                   request_line,
                                                   request_char))
            self.assertFalse(error_stream.str())
            completion_response = DataObject()
            self.assertTrue(example_server.handleCompletionRequest(completion_request,
                                                                   completion_response))
            self.assertFalse(example_server.getErrors())
            # Check body of json rpc from server completion response
            json_actual = stringstream()
            json_expect = '''
{
  "id" : 3
  ,"result" : {
    "isIncomplete" : false
    ,"items" : [
      {
      "deprecated" : false
      ,"detail" : "test detail 01"
      ,"documentation" : "test documentation 01"
      ,"insertTextFormat" : 1
      ,"kind" : 7
      ,"label" : "test label 01"
      ,"preselect" : false
      ,"textEdit" : {
        "newText" : "test new text 01"
        ,"range" : {
          "end" : {
          "character" : 61
          ,"line" : 23
        }
          ,"start" : {
            "character" : 45
            ,"line" : 23
          }
        }
      }
    }
      ,{
      "deprecated" : false
      ,"detail" : "test detail 02"
      ,"documentation" : "test documentation 02"
      ,"insertTextFormat" : 2
      ,"kind" : 12
      ,"label" : "test label 02"
      ,"preselect" : false
      ,"textEdit" : {
        "newText" : "test new text 02"
        ,"range" : {
          "end" : {
          "character" : 61
          ,"line" : 23
        }
          ,"start" : {
            "character" : 45
            ,"line" : 23
          }
        }
      }
    }
      ,{
      "deprecated" : false
      ,"detail" : "test detail 03"
      ,"documentation" : "test documentation 03"
      ,"insertTextFormat" : 1
      ,"kind" : 23
      ,"label" : "test label 03"
      ,"preselect" : false
      ,"textEdit" : {
        "newText" : "test new text 03"
        ,"range" : {
          "end" : {
          "character" : 61
          ,"line" : 23
        }
          ,"start" : {
            "character" : 45
            ,"line" : 23
          }
        }
      }
    }
    ]
  }
}
            '''
            self.assertTrue(completion_response.format_json(json_actual))
            self.assertEqual(json_expect.strip(), json_actual.str())
            # Check values dissected from server completion response
            completion_items = DataArray()
            success, server_response_id, is_inc = dissectCompletionResponse(completion_response,
                                                                            error_stream,
                                                                            completion_items)
            self.assertTrue(success)
            self.assertFalse(error_stream.str())
            self.assertEqual(client_request_id, server_response_id)
            self.assertFalse(is_inc)
            self.assertEqual(3, completion_items.size())
            for i in range(completion_items.size()):
                item = completion_items.at(i).to_object()
                label, text, detail, doc = string(), string(), string(), string()
                success, beg_line, beg_char, end_line, end_char, \
                kind, deprecated, preselect, format = \
                    dissectCompletionObject(item, error_stream, label, text, detail, doc)
                self.assertTrue(success)
                if i == 0:
                    self.assertEqual( "test label 01"         , label      )
                    self.assertEqual( "test new text 01"      , text       )
                    self.assertEqual( "test detail 01"        , detail     )
                    self.assertEqual( "test documentation 01" , doc        )
                    self.assertEqual( 23                      , beg_line   )
                    self.assertEqual( 45                      , beg_char   )
                    self.assertEqual( 23                      , end_line   )
                    self.assertEqual( 61                      , end_char   )
                    self.assertEqual( m_comp_kind_class       , kind       )
                    self.assertEqual( False                   , deprecated )
                    self.assertEqual( False                   , preselect  )
                    self.assertEqual( m_text_format_plaintext , format     )
                elif i == 1:
                    self.assertEqual( "test label 02"         , label      )
                    self.assertEqual( "test new text 02"      , text       )
                    self.assertEqual( "test detail 02"        , detail     )
                    self.assertEqual( "test documentation 02" , doc        )
                    self.assertEqual( 23                      , beg_line   )
                    self.assertEqual( 45                      , beg_char   )
                    self.assertEqual( 23                      , end_line   )
                    self.assertEqual( 61                      , end_char   )
                    self.assertEqual( m_comp_kind_value       , kind       )
                    self.assertEqual( False                   , deprecated )
                    self.assertEqual( False                   , preselect  )
                    self.assertEqual( m_text_format_snippet   , format     )
                elif i == 2:
                    self.assertEqual( "test label 03"         , label      )
                    self.assertEqual( "test new text 03"      , text       )
                    self.assertEqual( "test detail 03"        , detail     )
                    self.assertEqual( "test documentation 03" , doc        )
                    self.assertEqual( 23                      , beg_line   )
                    self.assertEqual( 45                      , beg_char   )
                    self.assertEqual( 23                      , end_line   )
                    self.assertEqual( 61                      , end_char   )
                    self.assertEqual( m_comp_kind_event       , kind       )
                    self.assertEqual( False                   , deprecated )
                    self.assertEqual( False                   , preselect  )
                    self.assertEqual( m_text_format_plaintext , format     )

        with self.subTest(msg='server_methods.definition'):
            # Build test definition request and use server to handle
            definition_request         = DataObject()
            document_path              = "test/path/to/doc"
            client_request_id          = 4
            request_line, request_char = 34, 56
            self.assertTrue(buildDefinitionRequest(definition_request,
                                                   error_stream,
                                                   client_request_id,
                                                   document_path,
                                                   request_line,
                                                   request_char))
            self.assertFalse(error_stream.str())
            definition_response = DataObject()
            self.assertTrue(example_server.handleDefinitionRequest(definition_request,
                                                                   definition_response))
            self.assertFalse(example_server.getErrors())
            # Check body of json rpc from server definition response
            json_actual = stringstream()
            json_expect = '''
{
  "id" : 4
  ,"result" : [
    {
    "range" : {
    "end" : {
    "character" : 31
    ,"line" : 15
  }
    ,"start" : {
      "character" : 21
      ,"line" : 15
    }
  }
    ,"uri" : "path/to/def/doc/01"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 32
    ,"line" : 25
  }
    ,"start" : {
      "character" : 22
      ,"line" : 25
    }
  }
    ,"uri" : "path/to/def/doc/02"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 33
    ,"line" : 35
  }
    ,"start" : {
      "character" : 23
      ,"line" : 35
    }
  }
    ,"uri" : "path/to/def/doc/03"
  }
  ]
}
            '''
            self.assertTrue(definition_response.format_json(json_actual))
            self.assertEqual(json_expect.strip(), json_actual.str())
            # Check values dissected from server definition response
            definition_locations = DataArray()
            success, server_response_id = dissectLocationsResponse(definition_response,
                                                                   error_stream,
                                                                   definition_locations)
            self.assertTrue(success)
            self.assertFalse(error_stream.str())
            self.assertEqual(client_request_id, server_response_id)
            self.assertEqual(3, definition_locations.size())
            for i in range(definition_locations.size()):
                location_object = definition_locations.at(i).to_object()
                doc_path = string()
                success, beg_line, beg_char, end_line, end_char = \
                    dissectLocationObject(location_object, error_stream, doc_path)
                self.assertTrue(success)
                if i == 0:
                    self.assertEqual( "path/to/def/doc/01" , doc_path )
                    self.assertEqual( 15                   , beg_line )
                    self.assertEqual( 21                   , beg_char )
                    self.assertEqual( 15                   , end_line )
                    self.assertEqual( 31                   , end_char )
                elif i == 1:
                    self.assertEqual( "path/to/def/doc/02" , doc_path )
                    self.assertEqual( 25                   , beg_line )
                    self.assertEqual( 22                   , beg_char )
                    self.assertEqual( 25                   , end_line )
                    self.assertEqual( 32                   , end_char )
                elif i == 2:
                    self.assertEqual( "path/to/def/doc/03" , doc_path )
                    self.assertEqual( 35                   , beg_line )
                    self.assertEqual( 23                   , beg_char )
                    self.assertEqual( 35                   , end_line )
                    self.assertEqual( 33                   , end_char )

        with self.subTest(msg='server_methods.references'):
            # Build test references request and use server to handle
            references_request         = DataObject()
            document_path              = "test/path/to/doc"
            client_request_id          = 5
            request_line, request_char = 45, 67
            include_decl               = True
            self.assertTrue(buildReferencesRequest(references_request,
                                                   error_stream,
                                                   client_request_id,
                                                   document_path,
                                                   request_line,
                                                   request_char,
                                                   include_decl))
            self.assertFalse(error_stream.str())
            references_response = DataObject()
            self.assertTrue(example_server.handleReferencesRequest(references_request,
                                                                   references_response))
            self.assertFalse(example_server.getErrors())
            # Check body of json rpc from server references response
            json_actual = stringstream()
            json_expect = '''
{
  "id" : 5
  ,"result" : [
    {
    "range" : {
    "end" : {
    "character" : 34
    ,"line" : 45
  }
    ,"start" : {
      "character" : 24
      ,"line" : 45
    }
  }
    ,"uri" : "path/to/ref/doc/04"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 35
    ,"line" : 55
  }
    ,"start" : {
      "character" : 25
      ,"line" : 55
    }
  }
    ,"uri" : "path/to/ref/doc/05"
  }
    ,{
    "range" : {
    "end" : {
    "character" : 36
    ,"line" : 65
  }
    ,"start" : {
      "character" : 26
      ,"line" : 65
    }
  }
    ,"uri" : "path/to/ref/doc/06"
  }
  ]
}
            '''
            self.assertTrue(references_response.format_json(json_actual))
            self.assertEqual(json_expect.strip(), json_actual.str())
            # Check values dissected from server references response
            references_locations = DataArray()
            success, server_response_id = dissectLocationsResponse(references_response,
                                                                   error_stream,
                                                                   references_locations)
            self.assertTrue(success)
            self.assertFalse(error_stream.str())
            self.assertEqual(client_request_id, server_response_id)
            self.assertEqual(3, references_locations.size())
            for i in range(references_locations.size()):
                location_object = references_locations.at(i).to_object()
                doc_path = string()
                success, beg_line, beg_char, end_line, end_char = \
                    dissectLocationObject(location_object, error_stream, doc_path)
                self.assertTrue(success)
                if i == 0:
                    self.assertEqual( "path/to/ref/doc/04" , doc_path )
                    self.assertEqual( 45                   , beg_line )
                    self.assertEqual( 24                   , beg_char )
                    self.assertEqual( 45                   , end_line )
                    self.assertEqual( 34                   , end_char )
                elif i == 1:
                    self.assertEqual( "path/to/ref/doc/05" , doc_path )
                    self.assertEqual( 55                   , beg_line )
                    self.assertEqual( 25                   , beg_char )
                    self.assertEqual( 55                   , end_line )
                    self.assertEqual( 35                   , end_char )
                elif i == 2:
                    self.assertEqual( "path/to/ref/doc/06" , doc_path )
                    self.assertEqual( 65                   , beg_line )
                    self.assertEqual( 26                   , beg_char )
                    self.assertEqual( 65                   , end_line )
                    self.assertEqual( 36                   , end_char )

        with self.subTest(msg='server_methods.formatting'):
            # Build test formatting request and use server to handle
            formatting_request = DataObject()
            document_path      = "test/path/to/doc"
            client_request_id  = 6
            tab_size           = 5
            insert_spaces      = True
            self.assertTrue(buildFormattingRequest(formatting_request,
                                                   error_stream,
                                                   client_request_id,
                                                   document_path,
                                                   tab_size,
                                                   insert_spaces))
            self.assertFalse(error_stream.str())
            formatting_response = DataObject()
            self.assertTrue(example_server.handleFormattingRequest(formatting_request,
                                                                   formatting_response))
            self.assertFalse(example_server.getErrors())
            # Check body of json rpc from server formatting response
            json_actual = stringstream()
            json_expect = r'''
{
  "id" : 6
  ,"result" : [
    {
    "newText" : "test\n     doc\n     text\n     02"
    ,"range" : {
      "end" : {
      "character" : 16
      ,"line" : 0
    }
      ,"start" : {
        "character" : 0
        ,"line" : 0
      }
    }
  }
  ]
}
            '''
            self.assertTrue(formatting_response.format_json(json_actual))
            self.assertEqual(json_expect.strip(), json_actual.str())
            # Check values dissected from server formatting response
            textedits_array = DataArray()
            success, server_response_id = dissectFormattingResponse(formatting_response,
                                                                    error_stream,
                                                                    textedits_array)
            self.assertTrue(success)
            self.assertFalse(error_stream.str())
            self.assertEqual(client_request_id, server_response_id)
            self.assertEqual(1, textedits_array.size())
            textedit = textedits_array.at(0).to_object()
            new_text = string()
            success, beg_line, beg_char, end_line, end_char = \
                dissectTextEditObject(textedit, error_stream, new_text)
            self.assertTrue(success)
            self.assertEqual( 0                                    , beg_line )
            self.assertEqual( 0                                    , beg_char )
            self.assertEqual( 0                                    , end_line )
            self.assertEqual( 16                                   , end_char )
            self.assertEqual( "test\n     doc\n     text\n     02" , new_text )

        with self.subTest(msg='server_methods.hover'):
            # Build test hover text request and use server to handle
            hover_request              = DataObject()
            document_path              = "test/path/to/doc"
            client_request_id          = 7
            request_line, request_char = 56, 78
            self.assertTrue(buildHoverRequest(hover_request,
                                              error_stream,
                                              client_request_id,
                                              document_path,
                                              request_line,
                                              request_char))
            self.assertFalse(error_stream.str())
            hover_response = DataObject()
            self.assertTrue(example_server.handleHoverRequest(hover_request,
                                                              hover_response))
            self.assertFalse(example_server.getErrors())
            # Check body of json rpc from server hover text response
            json_actual = stringstream()
            json_expect = '''
{
  "id" : 7
  ,"result" : {
    "contents" : "hover text of example test server"
  }
}
            '''
            self.assertTrue(hover_response.format_json(json_actual))
            self.assertEqual(json_expect.strip(), json_actual.str())
            # Check text string dissected from server hover response
            display_text = string()
            success, server_response_id = dissectHoverResponse(hover_response,
                                                               error_stream,
                                                               display_text)
            self.assertTrue(success)
            self.assertFalse(error_stream.str())
            self.assertEqual(client_request_id, server_response_id)
            self.assertEqual("hover text of example test server", display_text)

        with self.subTest(msg='server_methods.didclose'):
            # Build test didclose notification to handle with server
            didclose_notification = DataObject()
            document_path         = "test/path/to/doc"
            self.assertTrue(buildDidCloseNotification(didclose_notification,
                                                      error_stream,
                                                      document_path))
            self.assertFalse(error_stream.str())
            self.assertTrue(example_server.handleDidCloseNotification(didclose_notification))
            self.assertFalse(example_server.getErrors())

        with self.subTest(msg='server_methods.shutdown'):
            # Build test shutdown request and handle by using server
            shutdown_request  = DataObject()
            client_request_id = 8
            self.assertTrue(buildShutdownRequest(shutdown_request,
                                                 error_stream,
                                                 client_request_id))
            self.assertFalse(error_stream.str())
            shutdown_response = DataObject()
            self.assertTrue(example_server.isRunning())
            self.assertTrue(example_server.handleShutdownRequest(shutdown_request,
                                                                 shutdown_response))
            self.assertFalse(example_server.isRunning())
            self.assertFalse(example_server.getErrors())
            # Check json rpc body with shutdown response from server
            json_actual = stringstream()
            json_expect = '''
{
  "id" : 8
  ,"result" : {}
}
            '''
            self.assertTrue(shutdown_response.format_json(json_actual))
            self.assertEqual(json_expect.strip(), json_actual.str())
            # Check id value dissected from server shutdown response
            success, server_response_id = dissectShutdownResponse(shutdown_response,
                                                                  error_stream)
            self.assertTrue(success)
            self.assertFalse(error_stream.str())
            self.assertEqual(client_request_id, server_response_id)

        with self.subTest(msg='server_methods.exit'):
            # Build test exit notification for handling using server
            exit_notification = DataObject()
            self.assertTrue(buildExitNotification(exit_notification, error_stream))
            self.assertFalse(error_stream.str())
            self.assertTrue(example_server.handleExitNotification(exit_notification))
            self.assertFalse(example_server.getErrors())

if __name__ == '__main__':
    unittest.main()
