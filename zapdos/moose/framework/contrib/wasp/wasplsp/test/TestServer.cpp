#include "TestServer.h"

namespace wasp {
namespace lsp  {

bool TestServer::parseDocumentForDiagnostics(
                      DataArray & diagnosticsList )
{
    // create diagnostics for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    DataObject diagnostic;

    if (this->document_text == "test\ntext\n1\nstring\n")
    {
        pass &= buildDiagnosticObject( diagnostic         ,
                                       this->errors       ,
                                       20                 ,
                                       16                 ,
                                       20                 ,
                                       27                 ,
                                       1                  ,
                                       "test.code.11"     ,
                                       "test_source_11"   ,
                                       "Test message 11." );

        diagnosticsList.push_back(diagnostic);

        pass &= buildDiagnosticObject( diagnostic         ,
                                       this->errors       ,
                                       28                 ,
                                       56                 ,
                                       34                 ,
                                       27                 ,
                                       2                  ,
                                       "test.code.22"     ,
                                       "test_source_22"   ,
                                       "Test message 22." );

        diagnosticsList.push_back(diagnostic);

        pass &= buildDiagnosticObject( diagnostic         ,
                                       this->errors       ,
                                       47                 ,
                                       36                 ,
                                       47                 ,
                                       43                 ,
                                       3                  ,
                                       "test.code.33"     ,
                                       "test_source_33"   ,
                                       "Test message 33." );

        diagnosticsList.push_back(diagnostic);
    }

    else if (this->document_text == "test\ntext\n2\nstring\n")
    {
        pass &= buildDiagnosticObject( diagnostic         ,
                                       this->errors       ,
                                       67                 ,
                                       45                 ,
                                       68                 ,
                                       16                 ,
                                       4                  ,
                                       "test.code.44"     ,
                                       "test_source_44"   ,
                                       "Test message 44." );

        diagnosticsList.push_back(diagnostic);

        pass &= buildDiagnosticObject( diagnostic         ,
                                       this->errors       ,
                                       87                 ,
                                       17                 ,
                                       88                 ,
                                       12                 ,
                                       1                  ,
                                       "test.code.55"     ,
                                       "test_source_55"   ,
                                       "Test message 55." );

        diagnosticsList.push_back(diagnostic);
    }

    // set resource files of input that will be registered for unit testing
    if (client_watcher_support)
    {
        std::set<std::string> resource_uris;
        if (this->document_path == "test/document/uri/string")
        {
            resource_uris.insert({prefixUriScheme("/path/to/resource/file01.i"),
                                  prefixUriScheme("/path/to/resource/file02.i")});
        }
        setResourcesForBase(this->document_path, resource_uris);
    }

    return pass;
}

bool TestServer::gatherDocumentCompletionItems(
                      DataArray & completionItems  ,
                      bool      & is_incomplete    ,
                      int         line             ,
                      int         character        )
{
    // create completion items for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    DataObject completion_item;

    if ( line      == 4 &&
         character == 2 )
    {
        pass &= buildCompletionObject( completion_item        ,
                                       this->errors           ,
                                       "test-label-1"         ,
                                       11                     ,
                                       11                     ,
                                       11                     ,
                                       11                     ,
                                       "test-insert-text-1"   ,
                                       m_comp_kind_event      ,
                                       "test type info 1"     ,
                                       "test documentation 1" ,
                                       false                  ,
                                       true                   ,
                                       m_text_format_plaintext );

        completionItems.push_back(completion_item);

        pass &= buildCompletionObject( completion_item        ,
                                       this->errors           ,
                                       "test-label-2"         ,
                                       22                     ,
                                       22                     ,
                                       22                     ,
                                       22                     ,
                                       "test-insert-text-2"   ,
                                       m_comp_kind_operator   ,
                                       "test type info 2"     ,
                                       "test documentation 2" ,
                                       false                  ,
                                       false                  ,
                                       m_text_format_snippet  );

        completionItems.push_back(completion_item);

        pass &= buildCompletionObject( completion_item        ,
                                       this->errors           ,
                                       "test-label-3"         ,
                                       33                     ,
                                       33                     ,
                                       33                     ,
                                       33                     ,
                                       "test-insert-text-3"   ,
                                       m_comp_kind_type_param ,
                                       "test type info 3"     ,
                                       "test documentation 3" ,
                                       false                  ,
                                       false                  );

        completionItems.push_back(completion_item);
    }

    is_incomplete = false;

    return pass;
}

bool TestServer::gatherDocumentDefinitionLocations(
                      DataArray & definitionLocations ,
                      int         line                ,
                      int         character           )
{
    // create definition locations for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    DataObject location_object;

    if ( line      == 2 &&
         character == 5 )
    {
        pass &= buildLocationObject( location_object       ,
                                     this->errors          ,
                                     this->document_path   ,
                                     11                    ,
                                     11                    ,
                                     11                    ,
                                     11                    );

        definitionLocations.push_back(location_object);

        pass &= buildLocationObject( location_object       ,
                                     this->errors          ,
                                     this->document_path   ,
                                     22                    ,
                                     22                    ,
                                     22                    ,
                                     22                    );

        definitionLocations.push_back(location_object);

        pass &= buildLocationObject( location_object       ,
                                     this->errors          ,
                                     this->document_path   ,
                                     33                    ,
                                     33                    ,
                                     33                    ,
                                     33                    );

        definitionLocations.push_back(location_object);
    }

    return pass;
}

bool TestServer::getHoverDisplayText(
                      std::string & displayText ,
                      int           line          ,
                      int           character     )
{
    // create hover display text for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    if (line == 7 && character == 8)
    {
        displayText = "this is the hover text from the test server";
    }

    return true;
}

bool TestServer::gatherDocumentReferencesLocations(
                      DataArray & referencesLocations ,
                      int         line                ,
                      int         character           ,
                      bool        include_declaration )
{
    // create document references for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    DataObject location_object;

    if ( line                == 1     &&
         character           == 3     &&
         include_declaration == false )
    {
        pass &= buildLocationObject( location_object       ,
                                     this->errors          ,
                                     this->document_path   ,
                                     44                    ,
                                     44                    ,
                                     44                    ,
                                     44                    );

        referencesLocations.push_back(location_object);

        pass &= buildLocationObject( location_object       ,
                                     this->errors          ,
                                     this->document_path   ,
                                     55                    ,
                                     55                    ,
                                     55                    ,
                                     55                    );

        referencesLocations.push_back(location_object);
    }

    return pass;
}

bool TestServer::gatherDocumentFormattingTextEdits(
                      DataArray & formattingTextEdits ,
                      int         tab_size            ,
                      bool        insert_spaces       )
{
    // create document formatting text edits for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    DataObject textedit_object;

    if (tab_size == 4 && insert_spaces == true)
    {
        pass &= buildTextEditObject( textedit_object                      ,
                                     this->errors                         ,
                                     10                                   ,
                                     01                                   ,
                                     14                                   ,
                                     03                                   ,
                                     "test\n  new\n  text\n  format\n  1" );

        formattingTextEdits.push_back(textedit_object);

        pass &= buildTextEditObject( textedit_object                      ,
                                     this->errors                         ,
                                     20                                   ,
                                     01                                   ,
                                     24                                   ,
                                     03                                   ,
                                     "test\n  new\n  text\n  format\n  2" );

        formattingTextEdits.push_back(textedit_object);

        pass &= buildTextEditObject( textedit_object                      ,
                                     this->errors                         ,
                                     30                                   ,
                                     01                                   ,
                                     34                                   ,
                                     03                                   ,
                                     "test\n  new\n  text\n  format\n  3" );

        formattingTextEdits.push_back(textedit_object);
    }

    return pass;
}

bool TestServer::gatherDocumentSymbols(
                      DataArray & documentSymbols )
{
    // create document symbols for unit testing

    if (!this->is_initialized)
    {
        this->errors << m_error_prefix << "Server needs to be initialized" << std::endl;
        return false;
    }

    bool pass = true;

    if (this->document_path == "test/document/uri/string")
    {
        std::string o1_name                      = "test_symbol_name_object_1";
        std::string o1_detail                    = "test::symbol::detail::object::1";
        int         o1_kind                      = m_symbol_kind_object;
        bool        o1_deprecated                = false;
        int         o1_start_line                = 10;
        int         o1_start_character           = 11;
        int         o1_end_line                  = 10;
        int         o1_end_character             = 17;
        int         o1_selection_start_line      = 10;
        int         o1_selection_start_character = 13;
        int         o1_selection_end_line        = 10;
        int         o1_selection_end_character   = 15;

        std::string o2_name                      = "test_symbol_name_object_2";
        std::string o2_detail                    = "test::symbol::detail::object::2";
        int         o2_kind                      = m_symbol_kind_key;
        bool        o2_deprecated                = false;
        int         o2_start_line                = 20;
        int         o2_start_character           = 21;
        int         o2_end_line                  = 20;
        int         o2_end_character             = 27;
        int         o2_selection_start_line      = 20;
        int         o2_selection_start_character = 23;
        int         o2_selection_end_line        = 20;
        int         o2_selection_end_character   = 25;

        std::string o3_name                      = "test_symbol_name_object_3";
        std::string o3_detail                    = "test::symbol::detail::object::3";
        int         o3_kind                      = m_symbol_kind_null;
        bool        o3_deprecated                = false;
        int         o3_start_line                = 30;
        int         o3_start_character           = 31;
        int         o3_end_line                  = 30;
        int         o3_end_character             = 37;
        int         o3_selection_start_line      = 30;
        int         o3_selection_start_character = 33;
        int         o3_selection_end_line        = 30;
        int         o3_selection_end_character   = 35;

        std::string o4_name                      = "test_symbol_name_object_4";
        std::string o4_detail                    = "test::symbol::detail::object::4";
        int         o4_kind                      = m_symbol_kind_enum_member;
        bool        o4_deprecated                = false;
        int         o4_start_line                = 40;
        int         o4_start_character           = 41;
        int         o4_end_line                  = 40;
        int         o4_end_character             = 47;
        int         o4_selection_start_line      = 40;
        int         o4_selection_start_character = 43;
        int         o4_selection_end_line        = 40;
        int         o4_selection_end_character   = 45;

        std::string o5_name                      = "test_symbol_name_object_5";
        std::string o5_detail                    = "test::symbol::detail::object::5";
        int         o5_kind                      = m_symbol_kind_struct;
        bool        o5_deprecated                = false;
        int         o5_start_line                = 50;
        int         o5_start_character           = 51;
        int         o5_end_line                  = 50;
        int         o5_end_character             = 57;
        int         o5_selection_start_line      = 50;
        int         o5_selection_start_character = 53;
        int         o5_selection_end_line        = 50;
        int         o5_selection_end_character   = 55;

        std::string o6_name                      = "test_symbol_name_object_6";
        std::string o6_detail                    = "test::symbol::detail::object::6";
        int         o6_kind                      = m_symbol_kind_event;
        bool        o6_deprecated                = false;
        int         o6_start_line                = 60;
        int         o6_start_character           = 61;
        int         o6_end_line                  = 60;
        int         o6_end_character             = 67;
        int         o6_selection_start_line      = 60;
        int         o6_selection_start_character = 63;
        int         o6_selection_end_line        = 60;
        int         o6_selection_end_character   = 65;

        std::string o7_name                      = "test_symbol_name_object_7";
        std::string o7_detail                    = "test::symbol::detail::object::7";
        int         o7_kind                      = m_symbol_kind_operator;
        bool        o7_deprecated                = false;
        int         o7_start_line                = 70;
        int         o7_start_character           = 71;
        int         o7_end_line                  = 70;
        int         o7_end_character             = 77;
        int         o7_selection_start_line      = 70;
        int         o7_selection_start_character = 73;
        int         o7_selection_end_line        = 70;
        int         o7_selection_end_character   = 75;

        std::string o8_name                      = "test_symbol_name_object_8";
        std::string o8_detail                    = "test::symbol::detail::object::8";
        int         o8_kind                      = m_symbol_kind_type_param;
        bool        o8_deprecated                = false;
        int         o8_start_line                = 80;
        int         o8_start_character           = 81;
        int         o8_end_line                  = 80;
        int         o8_end_character             = 87;
        int         o8_selection_start_line      = 80;
        int         o8_selection_start_character = 83;
        int         o8_selection_end_line        = 80;
        int         o8_selection_end_character   = 85;

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

        DataObject object_1;

        pass &= buildDocumentSymbolObject( object_1                     ,
                                           errors                       ,
                                           o1_name                      ,
                                           o1_detail                    ,
                                           o1_kind                      ,
                                           o1_deprecated                ,
                                           o1_start_line                ,
                                           o1_start_character           ,
                                           o1_end_line                  ,
                                           o1_end_character             ,
                                           o1_selection_start_line      ,
                                           o1_selection_start_character ,
                                           o1_selection_end_line        ,
                                           o1_selection_end_character   );

        DataObject & object_2 = addDocumentSymbolChild( object_1 );

        pass &= buildDocumentSymbolObject( object_2                     ,
                                           errors                       ,
                                           o2_name                      ,
                                           o2_detail                    ,
                                           o2_kind                      ,
                                           o2_deprecated                ,
                                           o2_start_line                ,
                                           o2_start_character           ,
                                           o2_end_line                  ,
                                           o2_end_character             ,
                                           o2_selection_start_line      ,
                                           o2_selection_start_character ,
                                           o2_selection_end_line        ,
                                           o2_selection_end_character   );

        DataObject & object_3 = addDocumentSymbolChild( object_1 );

        pass &= buildDocumentSymbolObject( object_3                     ,
                                           errors                       ,
                                           o3_name                      ,
                                           o3_detail                    ,
                                           o3_kind                      ,
                                           o3_deprecated                ,
                                           o3_start_line                ,
                                           o3_start_character           ,
                                           o3_end_line                  ,
                                           o3_end_character             ,
                                           o3_selection_start_line      ,
                                           o3_selection_start_character ,
                                           o3_selection_end_line        ,
                                           o3_selection_end_character   );

        DataObject & object_4 = addDocumentSymbolChild( object_3  );

        pass &= buildDocumentSymbolObject( object_4                     ,
                                           errors                       ,
                                           o4_name                      ,
                                           o4_detail                    ,
                                           o4_kind                      ,
                                           o4_deprecated                ,
                                           o4_start_line                ,
                                           o4_start_character           ,
                                           o4_end_line                  ,
                                           o4_end_character             ,
                                           o4_selection_start_line      ,
                                           o4_selection_start_character ,
                                           o4_selection_end_line        ,
                                           o4_selection_end_character   );

        DataObject & object_5 = addDocumentSymbolChild( object_4  );

        pass &= buildDocumentSymbolObject( object_5                     ,
                                           errors                       ,
                                           o5_name                      ,
                                           o5_detail                    ,
                                           o5_kind                      ,
                                           o5_deprecated                ,
                                           o5_start_line                ,
                                           o5_start_character           ,
                                           o5_end_line                  ,
                                           o5_end_character             ,
                                           o5_selection_start_line      ,
                                           o5_selection_start_character ,
                                           o5_selection_end_line        ,
                                           o5_selection_end_character   );

        DataObject & object_6 = addDocumentSymbolChild( object_5  );

        pass &= buildDocumentSymbolObject( object_6                     ,
                                           errors                       ,
                                           o6_name                      ,
                                           o6_detail                    ,
                                           o6_kind                      ,
                                           o6_deprecated                ,
                                           o6_start_line                ,
                                           o6_start_character           ,
                                           o6_end_line                  ,
                                           o6_end_character             ,
                                           o6_selection_start_line      ,
                                           o6_selection_start_character ,
                                           o6_selection_end_line        ,
                                           o6_selection_end_character   );

        DataObject & object_7 = addDocumentSymbolChild( object_4  );

        pass &= buildDocumentSymbolObject( object_7                     ,
                                           errors                       ,
                                           o7_name                      ,
                                           o7_detail                    ,
                                           o7_kind                      ,
                                           o7_deprecated                ,
                                           o7_start_line                ,
                                           o7_start_character           ,
                                           o7_end_line                  ,
                                           o7_end_character             ,
                                           o7_selection_start_line      ,
                                           o7_selection_start_character ,
                                           o7_selection_end_line        ,
                                           o7_selection_end_character   );

        DataObject & object_8 = addDocumentSymbolChild( object_4  );

        pass &= buildDocumentSymbolObject( object_8                     ,
                                           errors                       ,
                                           o8_name                      ,
                                           o8_detail                    ,
                                           o8_kind                      ,
                                           o8_deprecated                ,
                                           o8_start_line                ,
                                           o8_start_character           ,
                                           o8_end_line                  ,
                                           o8_end_character             ,
                                           o8_selection_start_line      ,
                                           o8_selection_start_character ,
                                           o8_selection_end_line        ,
                                           o8_selection_end_character   );

        documentSymbols.push_back( object_1 );
    }

    return pass;
}

bool TestServer::gatherExtensionResponses(
                      wasp::DataArray   & extensionResponses ,
                      const std::string & extensionMethod    ,
                      int                 line               ,
                      int                 character          )
{
    DataObject extension_response;

    // create extension 01 responses for unit testing
    if (extensionMethod == "testMethod01")
    {
        if (this->document_path == "test/document/uri/string" &&
            line == 3 && character == 4)
        {
            // DataObject extension_response;

            extension_response["field_01"] = "word_01";
            extension_response["field_02"] = 111111111;
            extension_response["field_03"] = 1111.1111;
            extensionResponses.push_back(extension_response);

            extension_response["field_01"] = "word_02";
            extension_response["field_02"] = 222222222;
            extension_response["field_03"] = 2222.2222;
            extensionResponses.push_back(extension_response);

            extension_response["field_01"] = "word_03";
            extension_response["field_02"] = 333333333;
            extension_response["field_03"] = 3333.3333;
            extensionResponses.push_back(extension_response);
        }
    }

    // create extension 02 responses for unit testing
    else if (extensionMethod == "testMethod02")
    {
        if (this->document_path == "test/document/uri/string" &&
            line == 6 && character == 7)
        {
            DataArray extension_array_01;
            extension_array_01.push_back("aaa");
            extension_array_01.push_back("bbb");
            extension_array_01.push_back("ccc");
            extension_array_01.push_back("ddd");

            DataArray extension_array_02;
            extension_array_02.push_back(11111);
            extension_array_02.push_back(22222);
            extension_array_02.push_back(33333);
            extension_array_02.push_back(44444);

            DataArray extension_array_03;
            extension_array_03.push_back(11.11);
            extension_array_03.push_back(22.22);
            extension_array_03.push_back(33.33);
            extension_array_03.push_back(44.44);

            extension_response["name"] = "title";
            extension_response["kind"] = "lined";
            extension_response["xval"] = extension_array_01;
            extension_response["yval"] = extension_array_02;
            extension_response["stdv"] = extension_array_03;
            extensionResponses.push_back(extension_response);
        }
    }

    return true;
}

} // namespace lsp
} // namespace wasp
