// ======================================================================
//
// TODO: For multi-timezone countries (US, RU, CA, AU, BR, etc.) the entries
// below pick a representative timezone (often the capital's). When a
// foreign deployment targets one of these countries, replace this with a
// proper polygon lookup using e.g. timezone-boundary-builder data.
//
// ======================================================================

#include "CountryInfo.h"

#include <unordered_map>

namespace SmartMet
{
namespace Osm
{
namespace CountryInfo
{
namespace
{
// clang-format off
const std::unordered_map<std::string, std::string> kCountryNames = {
    {"AD", "Andorra"},        {"AE", "United Arab Emirates"},
    {"AF", "Afghanistan"},    {"AL", "Albania"},
    {"AM", "Armenia"},        {"AR", "Argentina"},
    {"AT", "Austria"},        {"AU", "Australia"},
    {"AZ", "Azerbaijan"},     {"BA", "Bosnia and Herzegovina"},
    {"BD", "Bangladesh"},     {"BE", "Belgium"},
    {"BG", "Bulgaria"},       {"BR", "Brazil"},
    {"BY", "Belarus"},        {"CA", "Canada"},
    {"CH", "Switzerland"},    {"CL", "Chile"},
    {"CN", "China"},          {"CO", "Colombia"},
    {"CY", "Cyprus"},         {"CZ", "Czech Republic"},
    {"DE", "Germany"},        {"DK", "Denmark"},
    {"DZ", "Algeria"},        {"EE", "Estonia"},
    {"EG", "Egypt"},          {"ES", "Spain"},
    {"ET", "Ethiopia"},       {"FI", "Finland"},
    {"FR", "France"},         {"GB", "United Kingdom"},
    {"GE", "Georgia"},        {"GR", "Greece"},
    {"HR", "Croatia"},        {"HU", "Hungary"},
    {"ID", "Indonesia"},      {"IE", "Ireland"},
    {"IL", "Israel"},         {"IN", "India"},
    {"IQ", "Iraq"},           {"IR", "Iran"},
    {"IS", "Iceland"},        {"IT", "Italy"},
    {"JO", "Jordan"},         {"JP", "Japan"},
    {"KE", "Kenya"},          {"KG", "Kyrgyzstan"},
    {"KR", "South Korea"},    {"KZ", "Kazakhstan"},
    {"LB", "Lebanon"},        {"LT", "Lithuania"},
    {"LU", "Luxembourg"},     {"LV", "Latvia"},
    {"MA", "Morocco"},        {"MD", "Moldova"},
    {"ME", "Montenegro"},     {"MK", "North Macedonia"},
    {"MN", "Mongolia"},       {"MT", "Malta"},
    {"MX", "Mexico"},         {"MY", "Malaysia"},
    {"NG", "Nigeria"},        {"NL", "Netherlands"},
    {"NO", "Norway"},         {"NP", "Nepal"},
    {"NZ", "New Zealand"},    {"PE", "Peru"},
    {"PH", "Philippines"},    {"PK", "Pakistan"},
    {"PL", "Poland"},         {"PT", "Portugal"},
    {"RO", "Romania"},        {"RS", "Serbia"},
    {"RU", "Russia"},         {"SA", "Saudi Arabia"},
    {"SE", "Sweden"},         {"SG", "Singapore"},
    {"SI", "Slovenia"},       {"SK", "Slovakia"},
    {"TH", "Thailand"},       {"TJ", "Tajikistan"},
    {"TM", "Turkmenistan"},   {"TN", "Tunisia"},
    {"TR", "Turkey"},         {"TW", "Taiwan"},
    {"UA", "Ukraine"},        {"US", "United States"},
    {"UZ", "Uzbekistan"},     {"VN", "Vietnam"},
    {"ZA", "South Africa"},
};

const std::unordered_map<std::string, std::string> kTimezones = {
    {"AD", "Europe/Andorra"},     {"AE", "Asia/Dubai"},
    {"AF", "Asia/Kabul"},         {"AL", "Europe/Tirane"},
    {"AM", "Asia/Yerevan"},       {"AR", "America/Argentina/Buenos_Aires"},
    {"AT", "Europe/Vienna"},      {"AU", "Australia/Sydney"},
    {"AZ", "Asia/Baku"},          {"BA", "Europe/Sarajevo"},
    {"BD", "Asia/Dhaka"},         {"BE", "Europe/Brussels"},
    {"BG", "Europe/Sofia"},       {"BR", "America/Sao_Paulo"},
    {"BY", "Europe/Minsk"},       {"CA", "America/Toronto"},
    {"CH", "Europe/Zurich"},      {"CL", "America/Santiago"},
    {"CN", "Asia/Shanghai"},      {"CO", "America/Bogota"},
    {"CY", "Asia/Nicosia"},       {"CZ", "Europe/Prague"},
    {"DE", "Europe/Berlin"},      {"DK", "Europe/Copenhagen"},
    {"DZ", "Africa/Algiers"},     {"EE", "Europe/Tallinn"},
    {"EG", "Africa/Cairo"},       {"ES", "Europe/Madrid"},
    {"ET", "Africa/Addis_Ababa"}, {"FI", "Europe/Helsinki"},
    {"FR", "Europe/Paris"},       {"GB", "Europe/London"},
    {"GE", "Asia/Tbilisi"},       {"GR", "Europe/Athens"},
    {"HR", "Europe/Zagreb"},      {"HU", "Europe/Budapest"},
    {"ID", "Asia/Jakarta"},       {"IE", "Europe/Dublin"},
    {"IL", "Asia/Jerusalem"},     {"IN", "Asia/Kolkata"},
    {"IQ", "Asia/Baghdad"},       {"IR", "Asia/Tehran"},
    {"IS", "Atlantic/Reykjavik"}, {"IT", "Europe/Rome"},
    {"JO", "Asia/Amman"},         {"JP", "Asia/Tokyo"},
    {"KE", "Africa/Nairobi"},     {"KG", "Asia/Bishkek"},
    {"KR", "Asia/Seoul"},         {"KZ", "Asia/Almaty"},
    {"LB", "Asia/Beirut"},        {"LT", "Europe/Vilnius"},
    {"LU", "Europe/Luxembourg"},  {"LV", "Europe/Riga"},
    {"MA", "Africa/Casablanca"},  {"MD", "Europe/Chisinau"},
    {"ME", "Europe/Podgorica"},   {"MK", "Europe/Skopje"},
    {"MN", "Asia/Ulaanbaatar"},   {"MT", "Europe/Malta"},
    {"MX", "America/Mexico_City"},{"MY", "Asia/Kuala_Lumpur"},
    {"NG", "Africa/Lagos"},       {"NL", "Europe/Amsterdam"},
    {"NO", "Europe/Oslo"},        {"NP", "Asia/Kathmandu"},
    {"NZ", "Pacific/Auckland"},   {"PE", "America/Lima"},
    {"PH", "Asia/Manila"},        {"PK", "Asia/Karachi"},
    {"PL", "Europe/Warsaw"},      {"PT", "Europe/Lisbon"},
    {"RO", "Europe/Bucharest"},   {"RS", "Europe/Belgrade"},
    {"RU", "Europe/Moscow"},      {"SA", "Asia/Riyadh"},
    {"SE", "Europe/Stockholm"},   {"SG", "Asia/Singapore"},
    {"SI", "Europe/Ljubljana"},   {"SK", "Europe/Bratislava"},
    {"TH", "Asia/Bangkok"},       {"TJ", "Asia/Dushanbe"},
    {"TM", "Asia/Ashgabat"},      {"TN", "Africa/Tunis"},
    {"TR", "Europe/Istanbul"},    {"TW", "Asia/Taipei"},
    {"UA", "Europe/Kyiv"},        {"US", "America/New_York"},
    {"UZ", "Asia/Tashkent"},      {"VN", "Asia/Ho_Chi_Minh"},
    {"ZA", "Africa/Johannesburg"},
};
// clang-format on
}  // namespace

std::string countryName(const std::string& iso2)
{
  auto it = kCountryNames.find(iso2);
  return it != kCountryNames.end() ? it->second : iso2;
}

std::string defaultTimezone(const std::string& iso2)
{
  auto it = kTimezones.find(iso2);
  return it != kTimezones.end() ? it->second : "UTC";
}

}  // namespace CountryInfo
}  // namespace Osm
}  // namespace SmartMet
