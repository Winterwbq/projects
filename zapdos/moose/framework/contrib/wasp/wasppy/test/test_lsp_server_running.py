import unittest, threading, os
from wasp import *
from .example_server import ExampleServer
from .py_process_connection import PyProcessConnection

class TestLspServerRunning(unittest.TestCase):

    def test_server_running(self):
        # Check example server running ThreadConnection then IOStreamConnection
        for connection_type in [ "threaded", "iostream" ]:

            with self.subTest(msg='server_running.start_run_and_connect_client',
                              connection_type=connection_type):
                # Run threaded or iostream connection server and connect client
                if connection_type == "threaded":
                    # Get connection from thread which starts server run method
                    example_server = ExampleServer()
                    example_server.setConnection(ThreadConnection(example_server))
                    connection = example_server.getConnection()
                    self.assertFalse(connection.isServerRunning())
                    running_thread = threading.Thread(target=example_server.run, daemon=True)
                    running_thread.start()
                elif connection_type == "iostream":
                    # Get connection from subprocess which executes server path
                    server_path = os.path.join(os.path.dirname(__file__), "example_server.py")
                    connection = PyProcessConnection()
                    self.assertFalse(connection.isServerRunning())
                    self.assertTrue(connection.startServer(server_path))
                testing_client = ClientImpl()
                self.assertFalse(testing_client.isConnected())
                self.assertTrue(testing_client.connect(connection))
                self.assertTrue(testing_client.isConnected())
                self.assertFalse(connection.getServerErrors())
                self.assertFalse(testing_client.getErrors())

            with self.subTest(msg='server_running.initialize_and_open_document',
                              connection_type=connection_type):
                # Initialize server and handle initialized using startUpCleanly
                self.assertTrue(testing_client.startUpCleanly())
                self.assertTrue(testing_client.getConnection().isServerRunning())
                self.assertFalse(testing_client.getConnection().getServerErrors())
                self.assertFalse(testing_client.getErrors())
                self.assertEqual(1, testing_client.getPreviousRequestID())
                # Open document which contains initial text by notifying server
                doc_path = "test/path/to/doc"
                doc_lang = "test_language_id"
                doc_text = "test doc text 01"
                self.assertFalse(testing_client.isDocumentOpen())
                self.assertTrue(testing_client.doDocumentOpen(doc_path, doc_lang, doc_text))
                self.assertTrue(testing_client.isDocumentOpen())
                self.assertEqual(1, testing_client.getCurrentDocumentVersion())
                self.assertFalse(testing_client.getConnection().getServerErrors())
                self.assertFalse(testing_client.getErrors())
                self.assertEqual(1, testing_client.getPreviousRequestID())
                # Check diagnostic list provided by server for initial document
                self.assertEqual(3, testing_client.getDiagnosticSize())
                for i in range(testing_client.getDiagnosticSize()):
                    diag = clientDiagnostic()
                    self.assertTrue(testing_client.getDiagnosticAt(i, diag))
                    if i == 0:
                        self.assertEqual( 11            , diag.start_line      )
                        self.assertEqual( 12            , diag.start_character )
                        self.assertEqual( 13            , diag.end_line        )
                        self.assertEqual( 14            , diag.end_character   )
                        self.assertEqual( 1             , diag.severity        )
                        self.assertEqual( "code.101"    , diag.code            )
                        self.assertEqual( "source_101"  , diag.source          )
                        self.assertEqual( "message 101" , diag.message         )
                    elif i == 1:
                        self.assertEqual( 21            , diag.start_line      )
                        self.assertEqual( 22            , diag.start_character )
                        self.assertEqual( 23            , diag.end_line        )
                        self.assertEqual( 24            , diag.end_character   )
                        self.assertEqual( 2             , diag.severity        )
                        self.assertEqual( "code.202"    , diag.code            )
                        self.assertEqual( "source_202"  , diag.source          )
                        self.assertEqual( "message 202" , diag.message         )
                    elif i == 2:
                        self.assertEqual( 31            , diag.start_line      )
                        self.assertEqual( 32            , diag.start_character )
                        self.assertEqual( 33            , diag.end_line        )
                        self.assertEqual( 34            , diag.end_character   )
                        self.assertEqual( 3             , diag.severity        )
                        self.assertEqual( "code.303"    , diag.code            )
                        self.assertEqual( "source_303"  , diag.source          )
                        self.assertEqual( "message 303" , diag.message         )

            with self.subTest(msg='server_running.update_text_and_send_changes',
                              connection_type=connection_type):
                # Change text within open document and notify server of updates
                doc_edit = "test doc text 02"
                self.assertEqual(1, testing_client.getCurrentDocumentVersion())
                self.assertTrue(testing_client.doDocumentChange(-1, -1, -1, -1, -1, doc_edit))
                self.assertEqual(2, testing_client.getCurrentDocumentVersion())
                self.assertFalse(testing_client.getConnection().getServerErrors())
                self.assertFalse(testing_client.getErrors())
                self.assertEqual(1, testing_client.getPreviousRequestID())
                # Check diagnostic list provided by server for updated document
                self.assertEqual(2, testing_client.getDiagnosticSize())
                for i in range(testing_client.getDiagnosticSize()):
                    diag = clientDiagnostic()
                    self.assertTrue(testing_client.getDiagnosticAt(i, diag))
                    if i == 0:
                        self.assertEqual( 41            , diag.start_line      )
                        self.assertEqual( 42            , diag.start_character )
                        self.assertEqual( 43            , diag.end_line        )
                        self.assertEqual( 44            , diag.end_character   )
                        self.assertEqual( 1             , diag.severity        )
                        self.assertEqual( "code.404"    , diag.code            )
                        self.assertEqual( "source_404"  , diag.source          )
                        self.assertEqual( "message 404" , diag.message         )
                    elif i == 1:
                        self.assertEqual( 51            , diag.start_line      )
                        self.assertEqual( 52            , diag.start_character )
                        self.assertEqual( 53            , diag.end_line        )
                        self.assertEqual( 54            , diag.end_character   )
                        self.assertEqual( 2             , diag.severity        )
                        self.assertEqual( "code.505"    , diag.code            )
                        self.assertEqual( "source_505"  , diag.source          )
                        self.assertEqual( "message 505" , diag.message         )

            with self.subTest(msg='server_running.request_completion_and_check',
                              connection_type=connection_type):
                # Request document completion for parameters specific to server
                req_line, req_char = 23, 45
                self.assertTrue(testing_client.doDocumentCompletion(req_line, req_char))
                self.assertFalse(testing_client.getConnection().getServerErrors())
                self.assertFalse(testing_client.getErrors())
                self.assertEqual(2, testing_client.getPreviousRequestID())
                # Check completion list provided in server response for request
                self.assertEqual(3, testing_client.getCompletionSize())
                for i in range(testing_client.getCompletionSize()):
                    comp = clientCompletion()
                    self.assertTrue(testing_client.getCompletionAt(i, comp))
                    if i == 0:
                        self.assertEqual( "test label 01"         , comp.label              )
                        self.assertEqual( "test new text 01"      , comp.new_text           )
                        self.assertEqual( "test detail 01"        , comp.detail             )
                        self.assertEqual( "test documentation 01" , comp.documentation      )
                        self.assertEqual( 23                      , comp.start_line         )
                        self.assertEqual( 45                      , comp.start_character    )
                        self.assertEqual( 23                      , comp.end_line           )
                        self.assertEqual( 61                      , comp.end_character      )
                        self.assertEqual( m_comp_kind_class       , comp.kind               )
                        self.assertEqual( False                   , comp.deprecated         )
                        self.assertEqual( False                   , comp.preselect          )
                        self.assertEqual( m_text_format_plaintext , comp.insert_text_format )
                    elif i == 1:
                        self.assertEqual( "test label 02"         , comp.label              )
                        self.assertEqual( "test new text 02"      , comp.new_text           )
                        self.assertEqual( "test detail 02"        , comp.detail             )
                        self.assertEqual( "test documentation 02" , comp.documentation      )
                        self.assertEqual( 23                      , comp.start_line         )
                        self.assertEqual( 45                      , comp.start_character    )
                        self.assertEqual( 23                      , comp.end_line           )
                        self.assertEqual( 61                      , comp.end_character      )
                        self.assertEqual( m_comp_kind_value       , comp.kind               )
                        self.assertEqual( False                   , comp.deprecated         )
                        self.assertEqual( False                   , comp.preselect          )
                        self.assertEqual( m_text_format_snippet   , comp.insert_text_format )
                    elif i == 2:
                        self.assertEqual( "test label 03"         , comp.label              )
                        self.assertEqual( "test new text 03"      , comp.new_text           )
                        self.assertEqual( "test detail 03"        , comp.detail             )
                        self.assertEqual( "test documentation 03" , comp.documentation      )
                        self.assertEqual( 23                      , comp.start_line         )
                        self.assertEqual( 45                      , comp.start_character    )
                        self.assertEqual( 23                      , comp.end_line           )
                        self.assertEqual( 61                      , comp.end_character      )
                        self.assertEqual( m_comp_kind_event       , comp.kind               )
                        self.assertEqual( False                   , comp.deprecated         )
                        self.assertEqual( False                   , comp.preselect          )
                        self.assertEqual( m_text_format_plaintext , comp.insert_text_format )

            with self.subTest(msg='server_running.request_definition_and_check',
                              connection_type=connection_type):
                # Request document definition for parameters specific to server
                req_line, req_char = 34, 56
                self.assertTrue(testing_client.doDocumentDefinition(req_line, req_char))
                self.assertFalse(testing_client.getConnection().getServerErrors())
                self.assertFalse(testing_client.getErrors())
                self.assertEqual(3, testing_client.getPreviousRequestID())
                # Check definition list provided in server response for request
                self.assertEqual(3, testing_client.getDefinitionSize())
                for i in range(testing_client.getDefinitionSize()):
                    dloc = clientDefinition()
                    self.assertTrue(testing_client.getDefinitionAt(i, dloc))
                    if i == 0:
                        self.assertEqual( "path/to/def/doc/01" , dloc.target_uri      )
                        self.assertEqual( 15                   , dloc.start_line      )
                        self.assertEqual( 21                   , dloc.start_character )
                        self.assertEqual( 15                   , dloc.end_line        )
                        self.assertEqual( 31                   , dloc.end_character   )
                    elif i == 1:
                        self.assertEqual( "path/to/def/doc/02" , dloc.target_uri      )
                        self.assertEqual( 25                   , dloc.start_line      )
                        self.assertEqual( 22                   , dloc.start_character )
                        self.assertEqual( 25                   , dloc.end_line        )
                        self.assertEqual( 32                   , dloc.end_character   )
                    elif i == 2:
                        self.assertEqual( "path/to/def/doc/03" , dloc.target_uri      )
                        self.assertEqual( 35                   , dloc.start_line      )
                        self.assertEqual( 23                   , dloc.start_character )
                        self.assertEqual( 35                   , dloc.end_line        )
                        self.assertEqual( 33                   , dloc.end_character   )

            with self.subTest(msg='server_running.request_references_and_check',
                              connection_type=connection_type):
                # Request document references for parameters specific to server
                req_line, req_char = 45, 67
                inc_decl           = True
                self.assertTrue(testing_client.doDocumentReferences(req_line, req_char, inc_decl))
                self.assertFalse(testing_client.getConnection().getServerErrors())
                self.assertFalse(testing_client.getErrors())
                self.assertEqual(4, testing_client.getPreviousRequestID())
                # Check references list provided in server response for request
                self.assertEqual(3, testing_client.getReferencesSize())
                for i in range(testing_client.getReferencesSize()):
                    rloc = clientReference()
                    self.assertTrue(testing_client.getReferencesAt(i, rloc))
                    if i == 0:
                        self.assertEqual( "path/to/ref/doc/04" , rloc.target_uri      )
                        self.assertEqual( 45                   , rloc.start_line      )
                        self.assertEqual( 24                   , rloc.start_character )
                        self.assertEqual( 45                   , rloc.end_line        )
                        self.assertEqual( 34                   , rloc.end_character   )
                    elif i == 1:
                        self.assertEqual( "path/to/ref/doc/05" , rloc.target_uri      )
                        self.assertEqual( 55                   , rloc.start_line      )
                        self.assertEqual( 25                   , rloc.start_character )
                        self.assertEqual( 55                   , rloc.end_line        )
                        self.assertEqual( 35                   , rloc.end_character   )
                    elif i == 2:
                        self.assertEqual( "path/to/ref/doc/06" , rloc.target_uri      )
                        self.assertEqual( 65                   , rloc.start_line      )
                        self.assertEqual( 26                   , rloc.start_character )
                        self.assertEqual( 65                   , rloc.end_line        )
                        self.assertEqual( 36                   , rloc.end_character   )

            with self.subTest(msg='server_running.request_formatting_and_check',
                              connection_type=connection_type):
                # Request document formatting for parameters specific to server
                tab_size      = 3
                insert_spaces = True
                self.assertTrue(testing_client.doDocumentFormatting(tab_size, insert_spaces))
                self.assertFalse(testing_client.getConnection().getServerErrors())
                self.assertFalse(testing_client.getErrors())
                self.assertEqual(5, testing_client.getPreviousRequestID())
                # Check formatting edit provided in server response for request
                self.assertEqual(1, testing_client.getFormattingSize())
                edit = clientFormatting()
                self.assertTrue(testing_client.getFormattingAt(0, edit))
                self.assertEqual( 0                              , edit.start_line      )
                self.assertEqual( 0                              , edit.start_character )
                self.assertEqual( 0                              , edit.end_line        )
                self.assertEqual( 16                             , edit.end_character   )
                self.assertEqual( "test\n   doc\n   text\n   02" , edit.new_text        )

            with self.subTest(msg='server_running.request_hover_text_and_check',
                              connection_type=connection_type):
                # Request document hover text for parameters specific to server
                req_line, req_char = 56, 78
                self.assertTrue(testing_client.doDocumentHover(req_line, req_char))
                self.assertFalse(testing_client.getConnection().getServerErrors())
                self.assertFalse(testing_client.getErrors())
                self.assertEqual(6, testing_client.getPreviousRequestID())
                # Check hover display text given in server response for request
                display_text = string()
                self.assertTrue(testing_client.getHoverText(display_text))
                self.assertFalse(testing_client.getErrors())
                self.assertEqual("hover text of example test server", display_text)

            with self.subTest(msg='server_running.get_symbols_and_use_iterator',
                              connection_type=connection_type):
                # Request hierarchical tree of symbols for document from server
                self.assertTrue(testing_client.doDocumentSymbols())
                self.assertFalse(testing_client.getConnection().getServerErrors())
                self.assertFalse(testing_client.getErrors())
                self.assertEqual(7, testing_client.getPreviousRequestID())
                # Use SymbolIterator to check hierarchical tree built by server
                iter = testing_client.getSymbolIterator()
                indices, counter, paths_actual = [0], 0, []
                while indices[-1] < iter.getChildSize():
                    iter.moveToChildAt(indices[-1])
                    indices.append(-1)
                    paths_actual.append(iter.getPath())
                    name, detail = string(), string()
                    success,      kind,         deprecated,                  \
                    beg_line,     beg_char,     end_line,     end_char,      \
                    beg_line_sel, beg_char_sel, end_line_sel, end_char_sel = \
                        iter.dissectCurrentSymbol(name, detail)
                    self.assertTrue(success)
                    if counter == 0:
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
                    elif counter == 1:
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
                    elif counter == 2:
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
                    counter += 1
                    if iter.getChildSize() == 0:
                        while indices[-1] + 1 == iter.getChildSize() and iter.moveToParent():
                            indices.pop()
                    indices[-1] += 1
                paths_expect = [ "/doc_root_name (11:12)"               ,
                                 "/doc_root_name/child_01_name (11:12)" ,
                                 "/doc_root_name/child_02_name (41:42)" ]
                self.assertEqual(paths_expect, paths_actual)

            with self.subTest(msg='server_running.close_document_and_finish_up',
                              connection_type=connection_type):
                # Close document and quit with wrapUpCleanly so server can exit
                self.assertTrue(testing_client.getConnection().isServerRunning())
                self.assertTrue(testing_client.isDocumentOpen())
                self.assertTrue(testing_client.isConnected())
                self.assertTrue(testing_client.wrapUpCleanly())
                if connection_type == "threaded":
                    running_thread.join()
                elif connection_type == "iostream":
                    self.assertTrue(connection.waitForFinished())
                self.assertFalse(testing_client.isConnected())
                self.assertFalse(testing_client.isDocumentOpen())
                self.assertFalse(testing_client.getConnection().isServerRunning())
                self.assertFalse(testing_client.getConnection().getServerErrors())
                self.assertFalse(testing_client.getErrors())
                self.assertEqual(8, testing_client.getPreviousRequestID())

if __name__ == '__main__':
    unittest.main()
