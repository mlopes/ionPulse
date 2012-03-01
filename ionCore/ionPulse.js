/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

Array.prototype.each = function(callback) {
    var i;
    for (i=0; i<this.length;i++) {
        callback(this[i]);
    }
}

["qt.core", "qt.gui", "qt.xml", "qt.svg", "qt.network",
 "qt.sql", "qt.opengl", "qt.webkit", "qt.xmlpatterns",
 "qt.uitools"].each(qs.script.importExtension);

debugger
