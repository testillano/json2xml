/*
 ________________________________________________
|     _                 ___                 _   |
|    (_)               |__ \               | |  |
|     _ ___  ___  _ __    ) |_  ___ __ ___ | |  |
|    | / __|/ _ \| '_ \  / /\ \/ / '_ ` _ \| |  |  Json-to-xml C++ only-header library
|    | \__ \ (_) | | | |/ /_ >  <| | | | | | |  |  Version 1.1.0
|    | |___/\___/|_| |_|____/_/\_\_| |_| |_|_|  |  https://github.com/testillano/json2xml
|   _/ |                                        |
|  |__/                                         |
|_______________________________________________|

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2020 Eduardo Ramos

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <ert/json2xml.hpp>


int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: json2xml <json file>" << std::endl;
        return 1;
    }

    std::ifstream ifs(argv[1]);
    if (!ifs) {
        std::cerr << "Cannot read file provided !" << std::endl;
        return 1;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();

    // Consume json to build xml:
    ert::JsonSaxConsumer consumer(2);
    if (!nlohmann::json::sax_parse(ss.str(), &consumer)) {
        std::cerr << "Conversion error: " << consumer.getError() << std::endl;
        return 1;
    }

    // output xml
    std::cout << consumer.getXmlString() << std::endl;
    return 0;
}
