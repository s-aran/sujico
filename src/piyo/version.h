#pragma once


struct Version
{
  static inline constexpr auto Major            = "0";
  static inline constexpr auto Minor            = "00";
  static inline constexpr auto Build            = "269";
  static inline constexpr auto Milestone        = "beta";
  static inline constexpr auto MilestoneVersion = "3";
  static inline constexpr auto VcsRevision      = "34";
  static inline constexpr auto VcsHash          = "72d0d0348def";
  static inline constexpr auto datetime         = "2019/12/31 23:48:47";
  static inline constexpr auto tag              = "tip";
  static inline constexpr auto VersionString    = "0.00.269 beta-3 (34:72d0d0348def)";


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

