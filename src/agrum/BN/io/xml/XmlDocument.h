/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

/**
 * @file
 * @brief Opaque facade over the vendored ticpp/tinyxml XML parser.
 *
 * BIFXMLBNReader and XDSLBNReader are the only consumers of ticpp/tinyxml
 * (base/external/tinyxml). That vendored code must never be edited to add
 * export attributes (same rule as base/external/lrslib): it stays compiled
 * only into agrumBN, and its types (ticpp::Document, ticpp::Element, ...)
 * never appear in any aGrUM-owned header -- this facade holds them behind an
 * opaque handle (XmlElement) / a PIMPL (XmlDocument), so no other TU ever
 * needs to see or link against ticpp directly. Under BUILD_SHARED_LIBS=ON,
 * only *this* facade is GUM_PUBLIC_BN-tagged and exported from agrumBN; core
 * and every other consumer link against it normally, exactly like any other
 * BN-owned symbol.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_XML_DOCUMENT_H
#define GUM_XML_DOCUMENT_H

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include <agrum/config.h>

namespace gum {

  /// Thrown by XmlDocument/XmlElement on any ticpp/tinyxml parse or access error.
  class GUM_PUBLIC_BN XmlException: public std::exception {
    public:
    explicit XmlException(std::string message);

    const char* what() const noexcept override;

    private:
    std::string _message_;
  };

  /**
   * @class XmlElement
   * @brief A non-owning handle to an XML element, opaque over ticpp::Element.
   *
   * Default-constructed (or returned by a failed non-throwing lookup) as
   * null: check with isNull() before use.
   */
  class GUM_PUBLIC_BN XmlElement {
    public:
    XmlElement() = default;

    bool isNull() const noexcept { return _handle_ == nullptr; }

    /// @throws XmlException if no such child exists.
    XmlElement firstChildElement(const std::string& tag) const;

    /// Returns a null XmlElement instead of throwing when throwIfNotFound is false.
    XmlElement firstChildElement(const std::string& tag, bool throwIfNotFound) const;

    /// All direct children with this tag, in document order.
    std::vector< XmlElement > children(const std::string& tag) const;

    std::string textOrDefault(const std::string& defaultValue) const;

    /// @throws XmlException if the attribute is missing.
    std::string attribute(const std::string& name) const;

    /// Returns false instead of throwing when the attribute is missing and
    /// throwIfNotFound is false; *out is left untouched in that case.
    bool attribute(const std::string& name, std::string* out, bool throwIfNotFound = false) const;

    private:
    friend class XmlDocument;

    explicit XmlElement(void* ticppElement) noexcept : _handle_(ticppElement) {}

    void* _handle_ = nullptr;   ///< opaque ticpp::Element*, never exposed
  };

  /**
   * @class XmlDocument
   * @brief An XML document, opaque over ticpp::Document.
   */
  class GUM_PUBLIC_BN XmlDocument {
    public:
    XmlDocument();
    explicit XmlDocument(const std::string& filePath);
    ~XmlDocument();

    XmlDocument(XmlDocument&&) noexcept;
    XmlDocument& operator=(XmlDocument&&) noexcept;
    XmlDocument(const XmlDocument&)            = delete;
    XmlDocument& operator=(const XmlDocument&) = delete;

    /// @throws XmlException on a parse error.
    void parse(const std::string& content);

    /// Loads the file passed to the constructor. @throws XmlException on failure.
    void loadFile();

    bool noChildren() const;

    /// @throws XmlException if no such child exists.
    XmlElement firstChildElement(const std::string& tag) const;

    private:
    struct Impl;
    std::unique_ptr< Impl > _pimpl_;
  };

} /* namespace gum */

#endif   // GUM_XML_DOCUMENT_H
