#ifndef WASPLSP_IOSTREAM_CONNECTION_H
#define WASPLSP_IOSTREAM_CONNECTION_H

#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include "wasplsp/LSP.h"
#include "wasplsp/Connection.h"
#include "wasplsp/ServerImpl.h"
#include "waspcore/decl.h"
#include "waspcore/wasp_bug.h"

namespace wasp {

class DataObject;

namespace lsp  {

/**
 * @brief The IOStreamConnection reads and writes standard input and output
 * It turns standard output off in construction and back on when destructed
 * It also toggles standard output on and off only to write each rpc string
 */
class WASP_PUBLIC IOStreamConnection : public Connection
{
  public:

    IOStreamConnection(ServerImpl * server)
      : server(server), cached_output_buffer(std::cout.rdbuf(nullptr)) {}

    IOStreamConnection(const IOStreamConnection &) = delete;

    IOStreamConnection & operator= (const IOStreamConnection &) = delete;

    ~IOStreamConnection()
    {
        // reset standard output stream buffer in order to turn it on again
        std::cout.rdbuf(cached_output_buffer);
    }

    typedef std::shared_ptr<wasp::lsp::Connection> SP;

    /** read packet string from standard input and store in provided object
     * @param object - reference to object to fill with data from read packet
     * @param errors - reference to error stream to pass to RPCStringToObject
     * @return - true if the read and conversion to object were successful
     */
    bool read( wasp::DataObject & object , std::ostream & errors )
    {
        bool pass = true;

        // read and check "Content-Length:" word from standard input

        std::string content_length_key;

        std::cin >> content_length_key;

        wasp_check( content_length_key == m_rpc_content_len_key );

        // read and check content length integer value from standard input

        int content_length_val;

        std::cin >> content_length_val;

        wasp_check( content_length_val > 0 );

        // gobble up all space until the next non-whitespace from standard input

        std::cin >> std::ws;

        // create a content string of size content_length_val

        std::string content( content_length_val , ' ' );

        // read exactly content_length_val bytes from standard input - blocking

        std::cin.read( &content[0] , content_length_val );

        // compose rpc_stream from from Content-Length: Length + \r\n + content

        std::stringstream rpc_stream;

        rpc_stream << content_length_key
                   << " "
                   << content_length_val
                   << m_rpc_separator
                   << content;

        // convert the composed full rpc_stream to the provided object reference

        pass &= RPCStringToObject( rpc_stream.str() , object , errors );

        return pass;
    }

    /** write data from provided object to standard output as packet string
     * @param object - object holding data to write to standard output
     * @param errors - reference to error stream to pass to objectToRPCString
     * @return - true if the conversion to the packet and write were successful
     */
    bool write( wasp::DataObject & object , std::ostream & errors )
    {
        bool pass = true;
        std::string rpc_string;

        // convert data from provided object into composed local rpc string
        pass &= objectToRPCString( object , rpc_string , errors );

        // reset standard output stream buffer so to write local rpc string
        std::cout.rdbuf(cached_output_buffer);

        // write local rpc string to standard output and force buffer flush
        std::cout << rpc_string << std::flush;

        // disable standard output again since local rpc string was written
        cached_output_buffer = std::cout.rdbuf(nullptr);

        return pass;
    }

    /** check if the server is initialized and reading from the connection
     * @return - true if server is initialized and reading from the connection
     */
    bool isServerRunning() const
    {
        return ( server != nullptr && server->isRunning() );
    }

    /** get all errors that have been stored on the server for any reason
     * @return - error string of all errors that have been stored on the server
     */
    std::string getServerErrors() const
    {
        return server->getErrors();
    }

  private:

    /**
     * @brief server - server used for this connection's reading and writing
     */
    ServerImpl * server;

    /**
     * @brief cached_output_buffer - standard output buffer saved for reset
     */
    std::streambuf * cached_output_buffer;
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_IOSTREAM_CONNECTION_H
