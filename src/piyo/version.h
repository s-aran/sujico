#pragma once


struct Version
{
  static inline constexpr auto Major            = "0";
  static inline constexpr auto Minor            = "00";
  static inline constexpr auto Build            = "293";
  static inline constexpr auto Milestone        = "beta";
  static inline constexpr auto MilestoneVersion = "3";
  static inline constexpr auto VcsHash          = "0970b7e";
  static inline constexpr auto datetime         = "2022/06/08 00:55:14";
  static inline constexpr auto VersionString    = "0.00.293 beta-3";


  static inline auto get(const std::string& pre = "", const std::string& post = "", const std::string& versionString = "version: ")
  {
    static std::stringstream ss;
    if (pre != "") ss << pre << " ";
    ss << versionString << VersionString;
    if (post != "") ss << " " << post;
    ss << std::endl;
    return ss.str();
  }
};

