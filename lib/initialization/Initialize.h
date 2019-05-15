/*
 * Copyright 2017-2019 Dmitry Ivanov
 *
 * This file is part of Quentier.
 *
 * Quentier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * Quentier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Quentier. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QUENTIER_LIB_INITIALIZATION_INITIALIZE_H
#define QUENTIER_LIB_INITIALIZATION_INITIALIZE_H

#include "CommandLineParser.h"

#include <QScopedPointer>

namespace quentier {

QT_FORWARD_DECLARE_CLASS(QuentierApplication)
QT_FORWARD_DECLARE_CLASS(Account)

class ParseCommandLineResult
{
public:
    ParseCommandLineResult() :
        m_shouldQuit(false),
        m_responseMessage(),
        m_errorDescription(),
        m_cmdOptions()
    {}

    bool            m_shouldQuit;
    QString         m_responseMessage;
    ErrorString     m_errorDescription;
    CommandLineParser::CommandLineOptions   m_cmdOptions;
};

void parseCommandLine(int argc, char *argv[], ParseCommandLineResult & result);

/**
 * Processes "storageDir" command line option. This command line option is special
 * because if it is present, it changes the base path where app stores nearly
 * all of its persistent data. Therefore this command line option needs to be
 * processed separately from others very early during the app initialization
 * routine.
 *
 * @param options           Command line arguments being searched for "storageDir"
 * @return                  True if no error was detected during the processing
 *                          of "storageDir" command line argument, false otherwise
 */
bool processStorageDirCommandLineOption(
    const CommandLineParser::CommandLineOptions & options);

/**
 * Processes "account" command line option, if it is present. The account being
 * parsed is the account which the app should use as the one loaded on startup
 *
 * @param options           Command line arguments being searched for "account"
 * @param pStartupAccount   Pointer to found account; if none is found or if no
 *                          "account" command line option is present, it would
 *                          be nullptr after the call
 * @return                  True if no error was detected during the processing
 *                          of "account" command line argument, false otherwise
 */
bool processAccountCommandLineOption(
    const CommandLineParser::CommandLineOptions & options,
    QScopedPointer<Account> & pStartupAccount);

/**
 * Processes "overrideSystemTrayAvailability" command line option, if it is
 * present.
 *
 * @param options           Command line arguments being searched for
 *                          "overrideSystemTrayAvailability"
 * @return                  True if no error was detected during the processing
 *                          of "overrideSystemTrayAvailability" command line
 *                          argument, false otherwise
 */
bool processOverrideSystemTrayAvailabilityCommandLineOption(
    const CommandLineParser::CommandLineOptions & options);

/**
 * Initializes various things Quentier requires before actually launching the app,
 * including parsing of command line arguments
 *
 * @param app               Quentier app instance
 * @param cmdOptions        Command line arguments to be parsed
 * @return                  True if no error was detected during
 *                          the initialization, false otherwise
 */
bool initialize(QuentierApplication & app,
                const CommandLineParser::CommandLineOptions & cmdOptions);

} // namespace quentier

#endif // QUENTIER_LIB_INITIALIZATION_INITIALIZE_H
