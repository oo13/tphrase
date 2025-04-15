/** parse function
    \file parse.cpp
    \author OOTA, Masato
    \copyright Copyright © 2024 OOTA, Masato
    \par License GPL-3.0-or-later or Apache-2.0
    \parblock
      This file is part of TPhrase.

      TPhrase is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      TPhrase is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with TPhrase.  If not, see <http://www.gnu.org/licenses/>.

      OR

      Licensed under the Apache License, Version 2.0 (the "License");
      you may not use TPhrase except in compliance with the License.
      You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

      Unless required by applicable law or agreed to in writing, software
      distributed under the License is distributed on an "AS IS" BASIS,
      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
      See the License for the specific language governing permissions and
      limitations under the License.
    \endparblock
*/

#include <limits>
#include <locale>
#include <stdexcept>
#include <string>
#include <utility>

#include "tphrase/common/InputIterator.h"
#include "CharFeeder.h"
#include "DataGsubs.h"
#include "DataOptions.h"
#include "DataProductionRule.h"
#include "DataSyntax.h"
#include "DataText.h"
#include "parse.h"

namespace {
    /** An exception for the parse error.

        parse() catches the exception, and handles it.
    */
    class ParseError : public std::runtime_error {
    public:
        /** The constructor.
            \param [in] what_arg The explanation of the exception.
        */
        explicit ParseError(const std::string &what_arg);
    };

    // Introduce some tphrase types into the local namespace.
    using CharFeeder = tphrase::CharFeeder;
    using DataGsubs = tphrase::DataGsubs;
    using DataOptions = tphrase::DataOptions;
    using DataProductionRule = tphrase::DataProductionRule;
    using DataSyntax = tphrase::DataSyntax;
    using DataText = tphrase::DataText;

    // Forward declarations
    void skip_space_nl(CharFeeder &it);
    void parse_assignment(CharFeeder &it, DataSyntax &syntax);
}

namespace tphrase {

    extern
    DataSyntax parse(InputIteratorBase &p, std::vector<std::string> &err_msg)
    {
        DataSyntax syntax;
        CharFeeder it{p};

        while (!it.is_end()) {
            try {
                parse_assignment(it, syntax);
            } catch (const ParseError &e) {
                err_msg.emplace_back(e.what());
                // Recovering from the error
                bool cont_line{false};
                for (; !it.is_end(); it.next()) {
                    const char c = it.getc();
                    if (c == '\n') {
                        if (cont_line) {
                            cont_line = false;
                        } else {
                            break;
                        }
                    } else if (c != ' ' && c != '\t') {
                        cont_line = c == '|' || c == '~' || c == '=';
                    }
                }
            }
        }
        syntax.fix_local_nonterminal(err_msg);
        return syntax;
    }
}

namespace {
    /** Utility function to generate a ParseError exception.
        \param [in] it The error position.
        \param [in] msg The error message.
        \note It throws an exception, so doesn't return.
    */
    void throw_parse_error(CharFeeder &it, const std::string &msg)
    {
        std::string e{"Line#"};
        e += std::to_string(it.get_line_number());
        e += ", Column#";
        e += std::to_string(it.get_column_number());
        e += ": ";
        e += msg;
        throw ParseError(e);
    }

    /** Skip spaces.
        \param [inout] it The character feeder.
        \param [in] en_nl Skip also the newlines if en_nl is true.
    */
    /*
      space_opt = [ { space } ] ;
      space = " " | "\t" | ( "{*", [ { ? [^}] ? } ], "}" ) ;
    */
    void skip_space(CharFeeder &it, const bool en_nl = false)
    {
        while (!it.is_end()) {
            const char c{it.getc()};
            if (c == '{' && it.get_nextc() == '*') {
                it.next();
                it.next();
                while (!it.is_end() && it.getc() != '}') {
                    it.next();
                }
                if (it.is_end()) {
                    throw_parse_error(it, "The end of the comment is expected.");
                }
            } else if (!(c == ' ' || c == '\t' || (en_nl && c == '\n'))) {
                break;
            }
            it.next();
        }
    }

    /** Skip spaces and newlines.
        \param [inout] it The character feeder.
    */
    /*
      space_nl_opt = [ { space | nl } ] ;
      nl = "\n" ;
    */
    void skip_space_nl(CharFeeder &it)
    {
        skip_space(it, true);
    }

    /** Skip spaces and a newline.
        \param [inout] it The character feeder.
    */
    /*
      space_one_nl_opt = space_opt, [ nl, space_opt ] ;
    */
    void skip_space_one_nl(CharFeeder &it)
    {
        skip_space(it);
        if (it.getc() == '\n') {
            it.next();
            skip_space(it);
        }
    }

    /** Can it be a part of a nonterminal?
        \param [in] c The character to be tested.
        \return It can be a part of a nonterminal.
    */
    bool is_nonterminal_char(const char c)
    {
        return std::isalnum(c, std::locale::classic()) || c == '_' || c == '.';
    }

    // Forward declarations
    std::string parse_nonterminal(CharFeeder &it);
    double parse_weight(CharFeeder &it);
    char parse_operator(CharFeeder &it);
    DataProductionRule parse_production_rule(CharFeeder &it, char term_char = '\0');

    /** Parse an assignment.
        \param [inout] it The character feeder.
        \param [inout] syntax The syntax into which the assignment is added.
    */
    /*
      start = space_nl_opt, [ { assignment, space_nl_opt } ], $ ;
      assignment = nonterminal, space_opt, [ weight, space_opt ], operator, space_one_nl_opt, production_rule, ( nl | $ ) ; (* One of spaces before weight is necessary because nonterminal consumes the numeric character and the period. *)
    */
    void parse_assignment(CharFeeder &it, DataSyntax &syntax)
    {
        skip_space_nl(it);
        if (it.is_end()) {
            return;
        }
        std::string nonterminal{parse_nonterminal(it)};
        skip_space(it);
        const double weight = parse_weight(it);
        skip_space(it);
        const char op_type{parse_operator(it)};
        skip_space_one_nl(it);
        DataProductionRule rule{parse_production_rule(it)};
        rule.set_weight(weight);
        if (it.is_end() || it.getc() == '\n') {
            if (op_type == ':') {
                rule.equalize_chance(true);
            }
            std::string err_msg;
            syntax.add(std::move(nonterminal), std::move(rule), err_msg);
            if (!err_msg.empty()) {
                throw_parse_error(it, err_msg);
            }
        } else {
            throw_parse_error(it, "The end of the text or \"\\n\" is expected.");
        }
    }

    /** Parse a nonterminal.
        \param [inout] it The character feeder.
        \return The nonterminal.
    */
    /*
      nonterminal = { ? [A-Za-z0-9_.] ? } ;
    */
    std::string parse_nonterminal(CharFeeder &it)
    {
        std::string nonterminal;
        while (!it.is_end()) {
            const char c{it.getc()};
            if (is_nonterminal_char(c)) {
                nonterminal += c;
                it.next();
            } else {
                break;
            }
        }
        if (nonterminal.empty()) {
            throw_parse_error(it, "A nonterminal \"[A-Za-z0-9_.]+\" is expected.");
        }
        return nonterminal;
    }

    /** Is it a character of the decimal number?
        \param [in] c The character to be tested.
        \return It's a decimal number.
     */
    bool is_decimal_number_char(const char c)
    {
        return '0' <= c && c <= '9';
    }

    /** Parse a weight number.
        \param [inout] it The character feeder.
        \return weight number if weight is specified, or NaN.
    */
    /*
      weight = ( ( { ? [0-9] ? }, [ "." ] ) | ( ".", ? [0-9] ? ) ), [ { ? [0-9] ? } ] ;
    */
    double parse_weight(CharFeeder &it)
    {
        std::string s;
        char c{it.getc()};
        if (c == '.') {
            it.next();
            c = it.getc();
            if (is_decimal_number_char(c)) {
                s += '.';
                s += c;
                it.next();
                c = it.getc();
            } else {
                throw_parse_error(it, "A number is expected. (\".\" is not a number.)");
            }
        } else if (is_decimal_number_char(c)) {
            do {
                s += c;
                it.next();
                c = it.getc();
            } while (is_decimal_number_char(c));
            if (c == '.') {
                s += c;
                it.next();
                c = it.getc();
            }
        } else {
            return std::numeric_limits<double>::quiet_NaN();
        }
        while (is_decimal_number_char(c)) {
            s += c;
            it.next();
            c = it.getc();
        }
        return std::stod(s);
    }

    /** Parse an operator.
        \param [inout] it The character feeder.
        \return ":" if the operator is ":=", or "=".
    */
    /*
      operator = "=" | ":=" ;
    */
    char parse_operator(CharFeeder &it)
    {
        const char c{it.getc()};
        if (c == '=') {
            it.next();
            return '=';
        } else if (c == ':') {
            it.next();
            if (it.getc() == '=') {
                it.next();
                return ':';
            } else {
                throw_parse_error(it, "\"=\" is expected.");
            }
        } else {
            throw_parse_error(it, "\"=\" or \":=\" is expected.");
        }
        return c;
    }

    // Forward declarations
    DataOptions parse_options(CharFeeder &it);
    DataGsubs parse_gsubs(CharFeeder &it);

    /** Parse a production rule.
        \param [inout] it The character feeder.
        \param [in] term_char The expected character after the production rule. If it's '\0', no special character is expected.
        \return The production rule.
    */
    /*
      production_rule = options, gsubs ;
    */
    DataProductionRule parse_production_rule(CharFeeder &it, const char term_char)
    {
        DataOptions options{parse_options(it)};
        DataGsubs gsubs{parse_gsubs(it)};
        DataProductionRule rule{std::move(options), std::move(gsubs)};
        if (term_char != '\0') {
            skip_space_nl(it);
            if (it.getc() == term_char) {
                it.next();
            } else {
                std::string s{"\""};
                s += term_char;
                s += "\" is expected.";
                throw_parse_error(it, s);
            }
        }
        return rule;
    }

    // Forward declaration
    DataText parse_text(CharFeeder &it);

    /** Parse an options.
        \param [inout] it The character feeder.
        \return The options.
    */
    /*
      options = text, space_opt, [ { "|", space_one_nl_opt, text, space_opt } ] ;
    */
    DataOptions parse_options(CharFeeder &it)
    {
        DataOptions options;
        options.add_text(parse_text(it));
        skip_space(it);
        while (it.getc() == '|') {
            it.next();
            skip_space_one_nl(it);
            options.add_text(parse_text(it));
            skip_space(it);
        }
        return options;
    }

    // Forward declarations
    DataText parse_quoted_text(CharFeeder &it);
    DataText parse_non_quoted_text(CharFeeder &it);
    void parse_expansion(CharFeeder &it, DataText &text, std::string &s);

    /** Parse a text.
        \param [inout] it The character feeder.
        \return The text.
    */
    /*
      text = text_begin, [ text_body, [ text_postfix ] ] |
             '"', [ { ? [^"{] ? | expansion } ], '"', space_opt, [ weight ] |
             "'", [ { ? [^'{] ? | expansion } ], "'", space_opt, [ weight ] |
             "`", [ { ? [^`{] ? | expansion } ], "`", space_opt, [ weight ] ;
      text_begin = ? [^ \t\n"'`|~{}] ? | expansion ; (* "}" is the next to the text when it's in {= ...}. *)
      expansion = "{", [ { ? [^}] ? } ], "}" ;
      weight = ( ( { ? [0-9] ? }, [ "." ] ) | ( ".", ? [0-9] ? ) ), [ { ? [0-9] ? } ] ;
    */
    DataText parse_text(CharFeeder &it)
    {
        const char c{it.getc()};
        if (it.is_end()
            || c == ' '
            || c == '\t'
            || c == '\n'
            || c == '|'
            || c == '~'
            || c == '}') {
            throw_parse_error(it, "A text is expected.");
            return DataText{};
        } else if (c == '"' || c == '\'' || c == '`') {
            return parse_quoted_text(it);
        } else {
            return parse_non_quoted_text(it);
        }
    }

    /** Parse a quoted text.
        \param [inout] it The character feeder.
        \return The text.
    */
    /*
      text = ...
          '"', [ { ? [^"{] ? | expansion } ], '"', space_opt, [ number ] |
          "'", [ { ? [^'{] ? | expansion } ], "'", space_opt, [ number ] |
          "`", [ { ? [^`{] ? | expansion } ], "`", space_opt, [ number ] ;
    */
    DataText parse_quoted_text(CharFeeder &it)
    {
        DataText text;
        std::string s;
        const char quote = it.getc();
        it.next();
        while (!it.is_end() && it.getc() != quote) {
            if (it.getc() == '{') {
                parse_expansion(it, text, s);
            } else {
                s += it.getc();
                it.next();
            }
        }
        if (it.is_end()) {
            std::string msg{"The end of the"};
            msg += quote;
            msg += "quoted text";
            msg += quote;
            msg += " is expected.";
            throw_parse_error(it, msg);
        }
        if (!s.empty()) {
            text.add_string(std::move(s));
        }
        it.next();
        skip_space(it);
        text.set_weight(parse_weight(it));
        return text;
    }

    /** Parse a non quoted text.
        \param [inout] it The character feeder.
        \return The text.
    */
    /*
      text = ...
          text_begin, [ text_body, [ text_postfix ] ] |
          ... ;
      text_body = { ? [^\n|~{}] ? | expansion } ;
      text_postfix = ? space_opt(?=($|[\n|~}])) ? ; (* text_postfix greedily matches with space_opt preceding the end of the text, newline, "|", "~", or "}", but it consumes only space_opt. *)
    */
    DataText parse_non_quoted_text(CharFeeder &it)
    {
        // The caller ensures it.getc() == text_begin or EOT.
        DataText text;
        std::string s;
        std::string spaces; // The candidate for "text_postfix" (trailing spaces)
        for (;;) {
            const char c = it.getc();
            if (it.is_end()
                || c == '\n'
                || c == '|'
                || c == '~'
                || c == '}') {
                if (!s.empty()) {
                    text.add_string(std::move(s));
                }
                break;
            } else if (c == ' ' || c == '\t') {
                spaces += c;
                it.next();
            } else {
                if (c == '{') {
                    if (it.get_nextc() == '*') {
                        // The comment block can match "text_postfix" rule, so don't clear "spaces" if it's a comment block.
                        it.next();
                        it.next();
                        while (!it.is_end() && it.getc() != '}') {
                            it.next();
                        }
                        if (it.is_end()) {
                            throw_parse_error(it, "The end of the comment is expected.");
                        }
                        it.next();
                    } else {
                        s += spaces;
                        spaces.clear();
                        parse_expansion(it, text, s);
                    }
                } else {
                    s += spaces;
                    s += c;
                    spaces.clear();
                    it.next();
                }
            }
        }
        return text;
    }

    /** Parse an expansion.
        \param [inout] it The character feeder.
        \param [inout] text The text into which the parts are added.
        \param [inout] s The unsolved string.
        \note Accomplish the definitive conversions here. (If the string enclosed by "{" and "}" may be a nonterminal, it's a non-definitive conversion.)
    */
    /*
      expansion = "{", [ { ? [^}] ? } ], "}" ;
    */
    void parse_expansion(CharFeeder &it, DataText &text, std::string &s)
    {
        it.next();
        const char c{it.getc()};
        if (it.get_nextc() == '}') {
            // "{(}" and "{)}"
            if (c == ')') {
                it.next();
                it.next();
                s += '}';
                return;
            } else if (c == '(') {
                it.next();
                it.next();
                s += '{';
                return;
            }
        }

        if (c == '=' || (c == ':' and it.get_nextc() == '=')) {
            // Anonymous production rule
            if (c == ':') {
                it.next();
            }
            it.next();
            skip_space_nl(it);
            text.add_string(s);
            s.clear();
            DataProductionRule rule{parse_production_rule(it, '}')};
            if (c == ':') {
                rule.equalize_chance();
            }
            text.add_anonymous_rule(std::move(rule));
            return;
        } else {
            const bool is_comment{c == '*'};
            bool is_nonterminal{c != '}' && !is_comment};
            std::string name;
            while (!it.is_end()) {
                const char c2{it.getc()};
                it.next();
                if (c2 == '}') {
                    if (is_nonterminal) {
                        if (!s.empty()) {
                            text.add_string(s);
                            s.clear();
                        }
                        text.add_expansion(std::move(name));
                        return;
                    } else if (is_comment) {
                        return;
                    } else {
                        s += name;
                        return;
                    }
                } else {
                    is_nonterminal = is_nonterminal && is_nonterminal_char(c2);
                    if (!is_comment) {
                        name += c2;
                    }
                }
            }
        }
        throw_parse_error(it, "The end of the brace expansion is expected.");
    }

    // Forward declarations
    std::string parse_pattern(CharFeeder &it, char sep, bool allow_empty);

    /** Parse a gsubs.
        \param [inout] it The character feeder.
        \return The gsubs.
    */
    /*
      gsubs = [ { "~", space_one_nl_opt, sep, { pat }, sep2, [ { pat } ], sep2, [ "g" ], space_opt } ] ; (* 'sep2' is the same character of 'sep'. *)
      sep = ? 7 bit character - [ \t\n{] ? ; (* '{' may be the beginning of the comment block. *)
    */
    DataGsubs parse_gsubs(CharFeeder &it)
    {
        DataGsubs gsubs;
        while (it.getc() == '~') {
            it.next();
            skip_space_one_nl(it);
            const char sep{it.getc()};
            if (it.is_end()) {
                throw_parse_error(it, "Unexpected EOT.");
            } else if (sep == '{') {
                throw_parse_error(it, "\"{\" isn't allowable as a separator.");
            } else if (static_cast<unsigned char>(sep) > 0x7F) {
                throw_parse_error(it, "The separator must be a 7 bit character.");
            }
            it.next();

            const std::string pattern{parse_pattern(it, sep, false)};
            const std::string repl{parse_pattern(it, sep, true)};
            const bool global{it.getc() == 'g'};
            if (global) {
                it.next();
            }
            try {
                gsubs.add_parameter(pattern, repl, global);
            } catch (const std::runtime_error &e) {
                std::string msg{"Gsub error: "};
                msg += e.what();
                throw_parse_error(it, msg);
            }
            skip_space(it);
        }
        return gsubs;
    }

    /** Parse a pattern or a repl.
        \param [inout] it The character feeder.
        \param [in] sep The separator character.
        \param [in] allow_empty Is the empty string allowed?
        \return The pattern or the repl.
    */
    /*
      pat = ? all characters ? - sep2 ; (* 'sep2' is the character precedes 'pat' in the parent 'gsubs'. *)
    */
    std::string parse_pattern(CharFeeder &it, const char sep, const bool allow_empty)
    {
        std::string pat;
        while (!it.is_end() && it.getc() != sep) {
            pat += it.getc();
            it.next();
        }
        if (!allow_empty && pat.empty()) {
            throw_parse_error(it, "A nonempty pattern is expected.");
        }
        if (it.is_end()) {
            throw_parse_error(it, "Unexpected EOT.");
        }
        it.next();
        return pat;
    }


    ParseError::ParseError(const std::string &what_arg)
        : std::runtime_error(what_arg)
    {
    }
}
