/*
 * Copyright 2019 Dmitry Ivanov
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

#include "ProcessTagOptions.h"

#include <QDebug>

namespace quentier {

bool processTagOptions(const CommandLineParser::CommandLineOptions & options,
                       quint32 & minTagsPerNote,
                       quint32 & maxTagsPerNote)
{
    minTagsPerNote = 0;
    maxTagsPerNote = 0;

    auto minTagsPerNoteIt = options.find(QStringLiteral("min-tags-per-note"));
    if (minTagsPerNoteIt != options.end())
    {
        bool conversionResult = false;
        quint32 value = minTagsPerNoteIt.value().toUInt(&conversionResult);
        if (!conversionResult) {
            qWarning() << "Failed to convert min tags per note to unsigned integer";
            return false;
        }

        minTagsPerNote = value;
    }

    auto maxTagsPerNoteIt = options.find(QStringLiteral("max-tags-per-note"));
    if (maxTagsPerNoteIt != options.end())
    {
        bool conversionResult = false;
        quint32 value = maxTagsPerNoteIt.value().toUInt(&conversionResult);
        if (!conversionResult) {
            qWarning() << "Failed to convert min tags per note to unsigned integer";
            return false;
        }

        maxTagsPerNote = value;
    }

    if (maxTagsPerNote < minTagsPerNote) {
        qWarning() << "Max tags per note should not be less than min tags per note";
        return false;
    }

    return true;
}

} // namespace quentier
