import sys, os
sys.path.insert(0, os.path.dirname(__file__)+"/..")
from wasp import *
import re

class ExampleServer(BaseServer):
    '''Concrete example server implementation meant for testing purposes'''

    def __init__(self):
        '''Set up capabilities of concrete example server implementation'''
        super().__init__()
        self.connection = None
        self.enableFullSync()
        self.enableSymbols()
        self.enableCompletion()
        self.enableDefinition()
        self.enableFormatting()
        self.enableReferences()
        self.enableHover()

    def setConnection(self, connection):
        '''Set connection object used by server for client communication'''
        self.connection = connection

    def getConnection(self):
        '''Get connection object used by server for client communication'''
        if not self.connection:
            self.errorStream() << m_error_prefix << "Server needs connection set\n"
            return None
        return self.connection

    def connectionRead(self, object):
        '''Read JSON-RPC from connection and put data in provided object'''
        if not self.connection:
            self.errorStream() << m_error_prefix << "Server needs connection set\n"
            return False
        return self.connection.read(object, self.errorStream())

    def connectionWrite(self, object):
        '''Convert object to JSON-RPC packet and write out to connection'''
        if not self.connection:
            self.errorStream() << m_error_prefix << "Server needs connection set\n"
            return False
        return self.connection.write(object, self.errorStream())

    def parseDocumentForDiagnostics(self, diagnostics_list):
        '''Process current document and add diagnostics to provided list'''
        success = True
        diagnostic = DataObject()

        if self.document_text == "test doc text 01":

            beg_line, beg_char, end_line, end_char, severity = 11, 12, 13, 14, 1
            code, src, msg = "code.101", "source_101", "message 101"
            success &= buildDiagnosticObject(diagnostic, self.errorStream(),
                beg_line, beg_char, end_line, end_char, severity, code, src, msg)
            diagnostics_list.push_back(diagnostic)

            beg_line, beg_char, end_line, end_char, severity = 21, 22, 23, 24, 2
            code, src, msg = "code.202", "source_202", "message 202"
            success &= buildDiagnosticObject(diagnostic, self.errorStream(),
                beg_line, beg_char, end_line, end_char, severity, code, src, msg)
            diagnostics_list.push_back(diagnostic)

            beg_line, beg_char, end_line, end_char, severity = 31, 32, 33, 34, 3
            code, src, msg = "code.303", "source_303", "message 303"
            success &= buildDiagnosticObject(diagnostic, self.errorStream(),
                beg_line, beg_char, end_line, end_char, severity, code, src, msg)
            diagnostics_list.push_back(diagnostic)

        elif self.document_text == "test doc text 02":

            beg_line, beg_char, end_line, end_char, severity = 41, 42, 43, 44, 1
            code, src, msg = "code.404", "source_404", "message 404"
            success &= buildDiagnosticObject(diagnostic, self.errorStream(),
                beg_line, beg_char, end_line, end_char, severity, code, src, msg)
            diagnostics_list.push_back(diagnostic)

            beg_line, beg_char, end_line, end_char, severity = 51, 52, 53, 54, 2
            code, src, msg = "code.505", "source_505", "message 505"
            success &= buildDiagnosticObject(diagnostic, self.errorStream(),
                beg_line, beg_char, end_line, end_char, severity, code, src, msg)
            diagnostics_list.push_back(diagnostic)

        return success

    def gatherDocumentSymbols(self, document_symbols):
        '''Build hierarchical symbol tree of document in provided object'''
        success = True
        document_symbols.push_back(DataObject())

        if self.document_text == "test doc text 02":

            symbol_doc_root = document_symbols.back().to_object()
            beg_line,     beg_char,     end_line,     end_char     = 11, 12, 61, 62
            beg_line_sel, beg_char_sel, end_line_sel, end_char_sel = 11, 12, 61, 62
            name, detail = "doc_root_name", "doc.root.detail"
            kind, deprecated = m_symbol_kind_object, False
            success &= buildDocumentSymbolObject(symbol_doc_root, self.errorStream(),
                name, detail, kind, deprecated, beg_line, beg_char, end_line, end_char,
                beg_line_sel, beg_char_sel, end_line_sel, end_char_sel)

            symbol_child_01 = addDocumentSymbolChild(symbol_doc_root)
            beg_line,    beg_char,      end_line,     end_char     = 11, 12, 31, 32
            beg_line_sel, beg_char_sel, end_line_sel, end_char_sel = 11, 12, 31, 32
            name, detail = "child_01_name", "child.01.detail"
            kind, deprecated = m_symbol_kind_struct, False
            success &= buildDocumentSymbolObject(symbol_child_01, self.errorStream(),
                name, detail, kind, deprecated, beg_line, beg_char, end_line, end_char,
                beg_line_sel, beg_char_sel, end_line_sel, end_char_sel)

            symbol_child_02 = addDocumentSymbolChild(symbol_doc_root)
            beg_line,     beg_char,     end_line,     end_char     = 41, 42, 61, 62
            beg_line_sel, beg_char_sel, end_line_sel, end_char_sel = 41, 42, 61, 62
            name, detail = "child_02_name", "child.02.detail"
            kind, deprecated = m_symbol_kind_method, False
            success &= buildDocumentSymbolObject(symbol_child_02, self.errorStream(),
                name, detail, kind, deprecated, beg_line, beg_char, end_line, end_char,
                beg_line_sel, beg_char_sel, end_line_sel, end_char_sel)

        return success

    def gatherDocumentCompletionItems(self, completion_items, req_line, req_char):
        '''Collect completion items for line and column in provided list'''
        success = True
        completion = DataObject()

        if req_line == 23 and req_char == 45:

            beg_line, beg_char, end_line, end_char = 23, 45, 23, 61
            label, doc = "test label 01", "test documentation 01"
            new_text, detail = "test new text 01", "test detail 01"
            kind, format = m_comp_kind_class, m_text_format_plaintext
            deprecated, preselect = False, False
            success &= buildCompletionObject(completion, self.errorStream(),
                label, beg_line, beg_char, end_line, end_char, new_text,
                kind, detail, doc, deprecated, preselect, format)
            completion_items.push_back(completion)

            beg_line, beg_char, end_line, end_char = 23, 45, 23, 61
            label, doc = "test label 02", "test documentation 02"
            new_text, detail = "test new text 02", "test detail 02"
            kind, format = m_comp_kind_value, m_text_format_snippet
            deprecated, preselect = False, False
            success &= buildCompletionObject(completion, self.errorStream(),
                label, beg_line, beg_char, end_line, end_char, new_text,
                kind, detail, doc, deprecated, preselect, format)
            completion_items.push_back(completion)

            beg_line, beg_char, end_line, end_char = 23, 45, 23, 61
            label, doc = "test label 03", "test documentation 03"
            new_text, detail = "test new text 03", "test detail 03"
            kind, format = m_comp_kind_event, m_text_format_plaintext
            deprecated, preselect = False, False
            success &= buildCompletionObject(completion, self.errorStream(),
                label, beg_line, beg_char, end_line, end_char, new_text,
                kind, detail, doc, deprecated, preselect, format)
            completion_items.push_back(completion)

        return success

    def gatherDocumentDefinitionLocations(self, definition_locations, req_line, req_char):
        '''Collect locations of definitions for request in provided list'''
        success = True
        definition = DataObject()

        if req_line == 34 and req_char == 56:

            beg_line, beg_char, end_line, end_char = 15, 21, 15, 31
            doc_path = "path/to/def/doc/01"
            success &= buildLocationObject(definition, self.errorStream(),
                doc_path, beg_line, beg_char, end_line, end_char)
            definition_locations.push_back(definition)

            beg_line, beg_char, end_line, end_char = 25, 22, 25, 32
            doc_path = "path/to/def/doc/02"
            success &= buildLocationObject(definition, self.errorStream(),
                doc_path, beg_line, beg_char, end_line, end_char)
            definition_locations.push_back(definition)

            beg_line, beg_char, end_line, end_char = 35, 23, 35, 33
            doc_path = "path/to/def/doc/03"
            success &= buildLocationObject(definition, self.errorStream(),
                doc_path, beg_line, beg_char, end_line, end_char)
            definition_locations.push_back(definition)

        return success

    def gatherDocumentReferencesLocations(self, references_locations, req_line, req_char, include_decl):
        '''Collect locations of references to request with provided list'''
        success = True
        reference = DataObject()

        if req_line == 45 and req_char == 67 and include_decl:

            beg_line, beg_char, end_line, end_char = 45, 24, 45, 34
            doc_path = "path/to/ref/doc/04"
            success &= buildLocationObject(reference, self.errorStream(),
                doc_path, beg_line, beg_char, end_line, end_char)
            references_locations.push_back(reference)

            beg_line, beg_char, end_line, end_char = 55, 25, 55, 35
            doc_path = "path/to/ref/doc/05"
            success &= buildLocationObject(reference, self.errorStream(),
                doc_path, beg_line, beg_char, end_line, end_char)
            references_locations.push_back(reference)

            beg_line, beg_char, end_line, end_char = 65, 26, 65, 36
            doc_path = "path/to/ref/doc/06"
            success &= buildLocationObject(reference, self.errorStream(),
                doc_path, beg_line, beg_char, end_line, end_char)
            references_locations.push_back(reference)

        return success

    def gatherDocumentFormattingTextEdits(self, formatting_textedits, tab_size, insert_spaces):
        '''Collect formatting edits to apply in order with provided list'''
        success = True
        textedit = DataObject()
        # Example format replaces whitespaces with indented newlines
        if insert_spaces:
            doc_lines = self.document_text.splitlines()
            beg_line, beg_char = 0, 0
            end_line, end_char = len(doc_lines)-1, len(doc_lines[-1])
            doc_format = re.sub(r"\s+", r"\n"+" "*tab_size, self.document_text)
            success &= buildTextEditObject(textedit, self.errorStream(),
                beg_line, beg_char, end_line, end_char, doc_format)
            formatting_textedits.push_back(textedit)
        return success

    def getHoverDisplayText(self, display_text, req_line, req_char):
        '''Return text that should be shown on hover at request position'''
        if req_line == 56 and req_char == 78:
            display_text = "hover text of example test server"
        return display_text

if __name__ == '__main__':
    example_server = ExampleServer()
    example_server.setConnection(IOStreamConnection(example_server))
    example_server.run()
