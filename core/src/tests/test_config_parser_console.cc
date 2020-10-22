/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2019-2019 Bareos GmbH & Co. KG

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation and included
   in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/
#if defined(HAVE_MINGW)
#include "include/bareos.h"
#include "gtest/gtest.h"
#else
#include "gtest/gtest.h"
#include "include/bareos.h"
#endif

#include "lib/parse_conf.h"
#include "console/console_globals.h"
#include "console/console_conf.h"

namespace console {

TEST(ConfigParser, test_console_config)
{
  OSDependentInit();
  InitMsg(NULL, NULL); /* initialize message handler */

  std::string path_to_config_file = std::string(
      RELATIVE_PROJECT_SOURCE_DIR "/configs/bareos-configparser-tests");

  my_config = InitConsConfig(path_to_config_file.c_str(), M_ERROR_TERM);
  my_config->ParseConfig();

  my_config->DumpResources(PrintMessage, NULL);

  delete my_config;

  TermMsg();         /* Terminate message handler */
  CloseMemoryPool(); /* release free memory in pool */
}

TEST(ConfigParser, test_console_dir_config_ucl_export)
{
  OSDependentInit();
  InitMsg(NULL, NULL); /* initialize message handler */

  std::string path_to_config_file = std::string(
      RELATIVE_PROJECT_SOURCE_DIR "/configs/bareos-configparser-tests");

  my_config = InitConsConfig(path_to_config_file.c_str(), M_ERROR_TERM);
  my_config->ParseConfig();

  std::cout << "Legacy dump of bconsole Director Resource: " << std::endl;
  my_config->DumpResources(PrintMessage, NULL);

  auto dir = my_config->GetResWithName(R_DIRECTOR, "backup-bareos-test-dir");
  ASSERT_TRUE(dir != nullptr);

  // export console::dir from config into ucl::Ucl object
  auto ucl = ucl::Ucl(std::map<std::string, ucl::Ucl>{
      {"Director", ucl::Ucl(dir->ExportToUcl())}});

  // dump Ucl Config
  std::cout << "Ucl config export: " << std::endl;
  std::string director_dump{ucl.dump(UCL_EMIT_CONFIG)};
  std::cout << director_dump << std::endl;

  // import Ucl Config from export into ucl::Ucl Object
  std::string error;
  auto ucl_imported = ucl::Ucl::parse(director_dump, error);

  ASSERT_TRUE(error.empty());
  EXPECT_TRUE(ucl == ucl_imported);

  // import last Ucl export into a new director resource
  DirectorResource dir_imported{};
  dir_imported.ImportFromUcl(
      ucl_imported["Director"]);  // currently creates mem leaks

  // export the imported config resource again
  auto ucl_export_again = ucl::Ucl(std::map<std::string, ucl::Ucl>{
      {"Director", ucl::Ucl(dir_imported.ExportToUcl())}});

  // dump imported config again
  std::cout << "Ucl config export after test-import: " << std::endl;
  std::string imported_director_dump{ucl_export_again.dump(UCL_EMIT_CONFIG)};
  std::cout << imported_director_dump << std::endl;

  EXPECT_TRUE(director_dump == imported_director_dump);

  delete my_config;

  TermMsg();         /* Terminate message handler */
  CloseMemoryPool(); /* release free memory in pool */
}

}  // namespace console
