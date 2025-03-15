#pragma once


struct Version
{
  static inline constexpr auto Major            = "0";
  static inline constexpr auto Minor            = "00";
  static inline constexpr auto Build            = "295";
  static inline constexpr auto Milestone        = "beta";
  static inline constexpr auto MilestoneVersion = "4";
  static inline constexpr auto VcsHash          = "e2301cd";
  static inline constexpr auto datetime         = "2025/03/15 17:09:59";
  static inline constexpr auto VersionString    = "0.00.295 beta-4";


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

