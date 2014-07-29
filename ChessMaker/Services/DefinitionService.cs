using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;
using System.Xml;

namespace ChessMaker.Services
{
    public class DefinitionService : ServiceBase
    {
        public VariantDefinition GetDefinition(VariantVersion version)
        {
            // at some point this should be made to cache definitions, but we have to make sure it updates the cache when they're modified
            return new VariantDefinition(version);
        }

        public bool SaveBoardData(VariantVersion version, string boardSVG, string cellLinks)
        {
            XmlDocument svgDoc = new XmlDocument();

            try
            {
                svgDoc.LoadXml(boardSVG);
            }
            catch
            {
                return false;
            }

            var definition = GetDefinition(version);
            var board = definition.CreateElement("board");

            var viewBox = definition.CreateAttribute("viewBox");
            viewBox.Value = svgDoc.DocumentElement.HasAttribute("viewBox") ? svgDoc.DocumentElement.Attributes["viewBox"].Value : definition.Board.Attributes["viewBox"].Value;
            board.Attributes.Append(viewBox);

            foreach (XmlNode node in svgDoc.DocumentElement.ChildNodes)
            {
                if (node.Name == "path")
                    SaveCellFromSVG(node, board);
                else if (node.Name == "line")
                    SaveLineFromSVG(node, board);
            }

            SaveLinkData(definition, board, cellLinks);
            definition.Board = board;
            Entities.SaveChanges();
            return true;
        }

        public void SaveLinkData(VariantVersion version, string linkData)
        {
            var definition = GetDefinition(version);
            var board = definition.Board;
            SaveLinkData(definition, board, linkData);
            definition.Board = board;
            Entities.SaveChanges();
        }

        private static void SaveCellFromSVG(XmlNode node, XmlNode board)
        {
            // nodes must have the "cell" class to count as a cell
            var classes = node.Attributes["class"];
            if (classes == null)
                return;
            var strClasses = " " + classes.Value + " ";
            if (!strClasses.Contains(" cell "))
                return;

            var definition = board.OwnerDocument;
            var cell = definition.CreateElement("cell");

            XmlAttribute attr;

            var id = node.Attributes["id"];
            if (id != null)
            {
                attr = definition.CreateAttribute("id");
                attr.Value = id.Value;
                cell.Attributes.Append(attr);
            }

            var color = strClasses.Contains(" light ") ? "light" : strClasses.Contains(" dark ") ? "dark" : "mid";

            if (strClasses.Contains(" lightStroke "))
                color += " lightStroke";
            else if (strClasses.Contains(" midStroke "))
                color += " midStroke";
            else if (strClasses.Contains(" darkStroke "))
                color += " darkStroke";

            attr = definition.CreateAttribute("color");
            attr.Value = color;
            cell.Attributes.Append(attr);

            var path = node.Attributes["d"];
            attr = definition.CreateAttribute("path");
            attr.Value = path.Value;
            cell.Attributes.Append(attr);

            board.AppendChild(cell);
        }

        private static void SaveLineFromSVG(XmlNode node, XmlNode board)
        {
            var definition = board.OwnerDocument;
            var line = definition.CreateElement("line");

            var classes = node.Attributes["class"];
            if (classes == null)
                return;
            var strClasses = " " + classes.Value + " ";
            var color = strClasses.Contains(" lightStroke ") ? "lightStroke" : strClasses.Contains(" darkStroke ") ? "darkStroke" : "midStroke";
            var attr = definition.CreateAttribute("color");
            attr.Value = color;
            line.Attributes.Append(attr);

            var sourceAttrs = new XmlAttribute[] { node.Attributes["x1"], node.Attributes["y1"], node.Attributes["x2"], node.Attributes["y2"] };
            foreach ( var sourceAttr in sourceAttrs )
                if (sourceAttr != null)
                {
                    var destAttr = definition.CreateAttribute(sourceAttr.Name);
                    destAttr.Value = sourceAttr.Value;
                    line.Attributes.Append(destAttr);
                }

            board.AppendChild(line);
        }

        public XmlDocument GetBoardSVG(VariantVersion version, bool addArrowheadDef = false)
        {
            var board = GetDefinition(version).Board;

            XmlDocument svgDoc = new XmlDocument();
            svgDoc.AppendChild(svgDoc.CreateElement("svg"));

            if (addArrowheadDef)
            {
                // add a <defs> section containing an arrowhead marker
                var defs = svgDoc.CreateElement("defs");
                defs.InnerXml = @"<marker id=""arrowhead"" viewbox=""0 0 10 10"" refX=""1"" refY=""5"" markerWidth=""6"" markerHeight=""6"" orient=""auto""><path d=""M 0 0 L 10 5 L 0 10 z"" /></marker>";
                svgDoc.DocumentElement.AppendChild(defs);
            }

            var attr = svgDoc.CreateAttribute("xmlns");
            attr.Value = "http://www.w3.org/2000/svg";
            svgDoc.DocumentElement.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("id");
            attr.Value = "render";
            svgDoc.DocumentElement.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("viewBox");
            attr.Value = board.Attributes["viewBox"].Value;
            svgDoc.DocumentElement.Attributes.Append(attr);

            foreach (XmlNode node in board.ChildNodes)
            {
                if (node.Name == "cell")
                    WriteCellToSVG(node, svgDoc);
                else if (node.Name == "line")
                    WriteLineToSVG(node, svgDoc);
            }

            return svgDoc;
        }

        private static void WriteCellToSVG(XmlNode node, XmlDocument svgDoc)
        {
            var cell = svgDoc.CreateElement("path");
            svgDoc.DocumentElement.AppendChild(cell);

            var attr = svgDoc.CreateAttribute("id");
            attr.Value = node.Attributes["id"].Value;
            cell.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("class");
            attr.Value = "cell " + node.Attributes["color"].Value;
            cell.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("d");
            attr.Value = node.Attributes["path"].Value;
            cell.Attributes.Append(attr);
        }

        private static void WriteLineToSVG(XmlNode node, XmlDocument svgDoc)
        {
            var line = svgDoc.CreateElement("line");
            svgDoc.DocumentElement.AppendChild(line);

            var attr = svgDoc.CreateAttribute("x1");
            attr.Value = node.Attributes["x1"].Value;
            line.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("y1");
            attr.Value = node.Attributes["y1"].Value;
            line.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("x2");
            attr.Value = node.Attributes["x2"].Value;
            line.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("y2");
            attr.Value = node.Attributes["y2"].Value;
            line.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("class");
            attr.Value = "detail " + node.Attributes["color"].Value;
            line.Attributes.Append(attr);
        }

        char[] outerSep = new char[] { ';' }, innerSep = new char[] { ':' };
        private void SaveLinkData(VariantDefinition def, XmlNode board, string cellLinks)
        {
            // clear existing links
            var existingLinks = board.SelectNodes("cell/link");
            foreach (XmlNode link in existingLinks)
                link.ParentNode.RemoveChild(link);

            var links = cellLinks.Split(outerSep, StringSplitOptions.RemoveEmptyEntries);
            foreach (string link in links)
            {
                var linkParts = link.Split(innerSep, 3);
                if (linkParts.Length != 3)
                    continue;

                // from, dir, to
                var fromNode = board.SelectSingleNode(string.Format("cell[@id='{0}']", linkParts[0]));
                var toNode = board.SelectSingleNode(string.Format("cell[@id='{0}']", linkParts[2]));

                if (fromNode == null || toNode == null)
                    continue;

                // add a child to fromNode like: <link dir="blah" to="blah" />
                var linkElem = def.CreateElement("link");
                linkElem.Attributes.Append(def.CreateAttribute("dir", linkParts[1]));
                linkElem.Attributes.Append(def.CreateAttribute("to", linkParts[2]));
                fromNode.AppendChild(linkElem);
            }
        }

        public string GetCellLinks(VariantVersion version)
        {
            var board = GetDefinition(version).Board;
            var sb = new StringBuilder();

            foreach (XmlNode cell in board.SelectNodes("cell"))
                foreach (XmlNode link in cell.SelectNodes("link"))
                    sb.AppendFormat(";{0}:{1}:{2}", cell.Attributes["id"].Value, link.Attributes["dir"].Value, link.Attributes["to"].Value);

            return sb.ToString();
        }

        private static readonly char[] space = { ' ' };
        public string CalculateGlobalDirectionDiagram(VariantVersion version)
        {
            SortedList<string, int> NumLinks = new SortedList<string,int>();
            SortedList<string, float> DXs = new SortedList<string,float>(), firstDX = new SortedList<string, float>();
            SortedList<string, float> DYs = new SortedList<string,float>(), firstDY = new SortedList<string, float>();

            var board = GetDefinition(version).Board;
            foreach (XmlNode fromCell in board.ChildNodes)
            {
                // read path attr for X & Y positions
                var fromCellPath = fromCell.Attributes["path"].Value.Split(space, 3);
                float fromCellX, fromCellY;
                if (!float.TryParse(fromCellPath[1], out fromCellY) || fromCellPath[0].Length < 2 || !float.TryParse(fromCellPath[0].Substring(1), out fromCellX))
                    continue;

                foreach (XmlNode link in fromCell.ChildNodes)
                {
                    var dir = link.Attributes["dir"].Value;
                    var toCellName = link.Attributes["to"].Value;

                    var toCell = board.SelectSingleNode(string.Format("cell[@id='{0}']", toCellName));
                    if (toCell == null)
                    {
                        if (!NumLinks.ContainsKey(dir))
                            NumLinks[dir] = 0;
                        continue;
                    }
                    
                    // read path attr for X & Y positions
                    var toCellPath = toCell.Attributes["path"].Value.Split(space, 3);
                    float toCellX, toCellY;
                    if (!float.TryParse(toCellPath[1], out toCellY) || toCellPath[0].Length < 2 || !float.TryParse(toCellPath[0].Substring(1), out toCellX))
                    {
                        if (!NumLinks.ContainsKey(dir))
                            NumLinks[dir] = 0;
                        continue;
                    }

                    var dx = toCellX - fromCellX;
                    var dy = toCellY - fromCellY;

                    int numLinks;
                    if (NumLinks.TryGetValue(dir, out numLinks) && numLinks > 0)
                    {
                        NumLinks[dir]++;
                        DXs[dir] += dx;
                        DYs[dir] += dy;
                    }
                    else
                    {
                        NumLinks[dir] = 1;
                        DXs[dir] = firstDX[dir] = dx;
                        DYs[dir] = firstDY[dir] = dy;
                    }
                }
            }

            var svgDoc = new XmlDocument();
            svgDoc.AppendChild(svgDoc.CreateElement("svg"));

            // add a <defs> section containing an arrowhead marker
            var defs = svgDoc.CreateElement("defs");
            defs.InnerXml = @"<marker id=""arrowhead"" viewbox=""0 0 10 10"" refX=""1"" refY=""5"" markerWidth=""6"" markerHeight=""6"" orient=""auto""><path d=""M 0 0 L 10 5 L 0 10 z"" /></marker>";
            svgDoc.DocumentElement.AppendChild(defs);

            var attr = svgDoc.CreateAttribute("xmlns");
            attr.Value = "http://www.w3.org/2000/svg";
            svgDoc.DocumentElement.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("id");
            attr.Value = "indicator";
            svgDoc.DocumentElement.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("viewbox");
            attr.Value = "-120 -120 240 240";
            svgDoc.DocumentElement.Attributes.Append(attr);

            const float tolerance = 0.01f;
            foreach(var kvp in NumLinks)
            {
                float dx, dy;

                if (kvp.Value == 0)
                {
                    dx = 0;
                    dy = 0;
                }
                else
                {
                    if (!DXs.TryGetValue(kvp.Key, out dx))
                        dx = 0;
                    else
                        dx /= kvp.Value;
                    
                    if (!DYs.TryGetValue(kvp.Key, out dy))
                        dy = 0;
                    else
                        dy /= kvp.Value;
                }

                if (Math.Abs(dx) < tolerance && Math.Abs(dy) < tolerance)
                {
                    dx = firstDX[kvp.Key]; dy = firstDY[kvp.Key];
                }

                var magnitude = (float)Math.Sqrt(dx * dx + dy * dy);
                dx /= magnitude; dy /= magnitude;

                var line = svgDoc.CreateElement("line");
                svgDoc.DocumentElement.AppendChild(line);

                attr = svgDoc.CreateAttribute("x1");
                attr.Value = (dx * 10).ToString("0.#");
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("y1");
                attr.Value = (dy * 10).ToString("0.#");
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("x2");
                attr.Value = (dx * 100).ToString("0.#");
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("y2");
                attr.Value = (dy * 100).ToString("0.#");
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("class");
                attr.Value = "marker";
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("dir");
                attr.Value = kvp.Key;
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("marker-end");
                attr.Value = "url(#arrowhead)";
                line.Attributes.Append(attr);
            }

            return svgDoc.OuterXml;
        }

        public string GetRelativeDirs(VariantVersion version)
        {
            return string.Empty;// throw new NotImplementedException();
        }
    }
}