#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <mutex>
#include <cstdlib>
#include <map>
#include <exception>
#include <stdexcept>
#include <type_traits>
#include <functional>

#include <cstdint>

#include <boost/assign.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/regex.hpp>

#include <toml.hpp>



#if defined _UNIX
// #include <sys/wait.h>
#elif defined _WIN32
#include <windows.h>
#endif

#ifdef GTEST
#include <gtest/gtest.h>
#pragma comment(lib, "gtest")
#endif  /* GTEST */
