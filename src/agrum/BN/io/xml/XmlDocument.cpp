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

// The only TU in aGrUM (besides ticpp's own sources) allowed to include the
// vendored parser -- see XmlDocument.h's file comment.
#include <agrum/base/external/tinyxml/ticpp/ticpp.h>
#include <agrum/BN/io/xml/XmlDocument.h>

namespace gum {

  XmlException::XmlException(std::string message) : _message_(std::move(message)) {}

  const char* XmlException::what() const noexcept { return _message_.c_str(); }

  XmlElement XmlElement::firstChildElement(const std::string& tag) const {
    auto* self = static_cast< ticpp::Element* >(_handle_);
    try {
      return XmlElement(self->FirstChildElement(tag));
    } catch (ticpp::Exception& e) { throw XmlException(e.what()); }
  }

  XmlElement XmlElement::firstChildElement(const std::string& tag, bool throwIfNotFound) const {
    auto* self = static_cast< ticpp::Element* >(_handle_);
    try {
      return XmlElement(self->FirstChildElement(tag, throwIfNotFound));
    } catch (ticpp::Exception& e) { throw XmlException(e.what()); }
  }

  std::vector< XmlElement > XmlElement::children(const std::string& tag) const {
    std::vector< XmlElement > result;
    if (_handle_ == nullptr) { return result; }
    auto*                             parent = static_cast< ticpp::Element* >(_handle_);
    ticpp::Iterator< ticpp::Element > it(tag);
    for (it = it.begin(parent); it != it.end(); ++it) {
      result.push_back(XmlElement(it.Get()));
    }
    return result;
  }

  std::string XmlElement::textOrDefault(const std::string& defaultValue) const {
    auto* self = static_cast< ticpp::Element* >(_handle_);
    return self->GetTextOrDefault(defaultValue);
  }

  std::string XmlElement::attribute(const std::string& name) const {
    auto* self = static_cast< ticpp::Element* >(_handle_);
    try {
      return self->GetAttribute(name);
    } catch (ticpp::Exception& e) { throw XmlException(e.what()); }
  }

  bool
      XmlElement::attribute(const std::string& name, std::string* out, bool throwIfNotFound) const {
    auto* self = static_cast< ticpp::Element* >(_handle_);
    // ticpp's GetAttribute(name, out, false) returns silently (no exception,
    // *out untouched) when the attribute is missing, so the try/catch below
    // never fires on that path -- HasAttribute is the only reliable signal.
    const bool found = self->HasAttribute(name);
    try {
      self->GetAttribute(name, out, throwIfNotFound);
    } catch (ticpp::Exception& e) { throw XmlException(e.what()); }
    return found;
  }

  struct XmlDocument::Impl {
    ticpp::Document doc;

    Impl() = default;

    explicit Impl(const std::string& filePath) : doc(filePath) {}
  };

  XmlDocument::XmlDocument() : _pimpl_(std::make_unique< Impl >()) {}

  XmlDocument::XmlDocument(const std::string& filePath) :
      _pimpl_(std::make_unique< Impl >(filePath)) {}

  XmlDocument::~XmlDocument()                                 = default;
  XmlDocument::XmlDocument(XmlDocument&&) noexcept            = default;
  XmlDocument& XmlDocument::operator=(XmlDocument&&) noexcept = default;

  void XmlDocument::parse(const std::string& content) {
    try {
      _pimpl_->doc.Parse(content);
    } catch (ticpp::Exception& e) { throw XmlException(e.what()); }
  }

  void XmlDocument::loadFile() {
    try {
      _pimpl_->doc.LoadFile();
    } catch (ticpp::Exception& e) { throw XmlException(e.what()); }
  }

  bool XmlDocument::noChildren() const { return _pimpl_->doc.NoChildren(); }

  XmlElement XmlDocument::firstChildElement(const std::string& tag) const {
    try {
      return XmlElement(_pimpl_->doc.FirstChildElement(tag));
    } catch (ticpp::Exception& e) { throw XmlException(e.what()); }
  }

} /* namespace gum */
