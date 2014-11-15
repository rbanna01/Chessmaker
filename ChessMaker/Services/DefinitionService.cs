using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Web;
using System.Xml;
using System.Xml.Schema;

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
            version.LastModified = DateTime.Now;
            Entities.SaveChanges();
            return true;
        }

        public void SaveLinkData(VariantVersion version, string linkData)
        {
            var definition = GetDefinition(version);
            var board = definition.Board;
            SaveLinkData(definition, board, linkData);
            definition.Board = board;
            version.LastModified = DateTime.Now;
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

            var fill = strClasses.Contains(" light ") ? "light" : strClasses.Contains(" dark ") ? "dark" : "mid";

            var border = string.Empty;
            if (strClasses.Contains(" lightStroke "))
                border = "light";
            else if (strClasses.Contains(" midStroke "))
                border = " mid";
            else if (strClasses.Contains(" darkStroke "))
                border = " dark";

            attr = definition.CreateAttribute("fill");
            attr.Value = fill;
            cell.Attributes.Append(attr);

            if (border != string.Empty)
            {
                attr = definition.CreateAttribute("border");
                attr.Value = fill;
                cell.Attributes.Append(attr);
            }

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
            var color = strClasses.Contains(" lightStroke ") ? "light" : strClasses.Contains(" darkStroke ") ? "dark" : "mid";
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

        public XmlDocument GetBoardSVG(VariantVersion version, bool addArrowheadDef = false, bool showCellRefs = false)
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

            if (showCellRefs) // done last, so that references appear on top of lines
                foreach (XmlNode node in board.SelectNodes("cell"))
                    WriteCellRefToSVG(node, svgDoc);

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
            attr.Value = "cell " + node.Attributes["fill"].Value;

            var borderAttr = node.Attributes["border"];
            if (borderAttr != null)
                attr.Value += " " + borderAttr.Value + "Stroke";
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
            attr.Value = "detail " + node.Attributes["color"].Value + "Stroke";
            line.Attributes.Append(attr);
        }

        private void WriteCellRefToSVG(XmlNode node, XmlDocument svgDoc)
        {
            var text = svgDoc.CreateElement("text");
            text.InnerText = node.Attributes["id"].Value;
            svgDoc.DocumentElement.AppendChild(text);

            var cellPathParts = node.Attributes["path"].Value.Substring(1).Split(space, 3);
            float cellX, cellY;
            if (!float.TryParse(cellPathParts[1], out cellY) || !float.TryParse(cellPathParts[0], out cellX))
                return;

            var attr = svgDoc.CreateAttribute("x");
            attr.Value = cellX.ToString();
            text.Attributes.Append(attr);

            attr = svgDoc.CreateAttribute("y");
            attr.Value = cellY.ToString();
            text.Attributes.Append(attr);
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

        private class GlobalDirInfo
        {
            public GlobalDirInfo(string name, int num = 1) { Name = name; Num = num; }
            public string Name { get; set; }
            public int Num { get; set; }

            public float DX { get; set; }
            public float DY { get; set; }

            public float FirstDX { get; set; }
            public float FirstDY { get; set; }

            public double Angle { get { return Math.Atan2(-DY, -DX); } }
        }

        private static readonly char[] space = { ' ' };
        public string CalculateGlobalDirectionDiagram(VariantVersion version)
        {
            var directions = new SortedList<string, GlobalDirInfo>();

            var board = GetDefinition(version).Board;
            foreach (XmlNode fromCell in board.SelectNodes("cell"))
            {
                // read path attr for X & Y positions
                var fromCellPath = fromCell.Attributes["path"].Value.Substring(1).Split(space, 3);
                float fromCellX, fromCellY;
                if (!float.TryParse(fromCellPath[1], out fromCellY) || fromCellPath[0].Length < 2 || !float.TryParse(fromCellPath[0], out fromCellX))
                    continue;

                foreach (XmlNode link in fromCell.ChildNodes)
                {
                    var dir = link.Attributes["dir"].Value;
                    var toCellName = link.Attributes["to"].Value;

                    var toCell = board.SelectSingleNode(string.Format("cell[@id='{0}']", toCellName));
                    if (toCell == null)
                    {
                        if (!directions.ContainsKey(dir))
                            directions[dir] = new GlobalDirInfo(dir, 0);
                        continue;
                    }

                    // read path attr for X & Y positions
                    var toCellPath = toCell.Attributes["path"].Value.Split(space, 3);
                    float toCellX, toCellY;
                    if (!float.TryParse(toCellPath[1], out toCellY) || !float.TryParse(toCellPath[0].Substring(1), out toCellX))
                    {
                        if (!directions.ContainsKey(dir))
                            directions[dir] = new GlobalDirInfo(dir, 0);
                        continue;
                    }

                    var dx = toCellX - fromCellX;
                    var dy = toCellY - fromCellY;

                    if (!directions.ContainsKey(dir))
                        directions[dir] = new GlobalDirInfo(dir) { FirstDX = dx, FirstDY = dy };

                    var direction = directions[dir];
                    direction.Num++;
                    direction.DX += dx;
                    direction.DY += dy;
                }
            }

            var svgDoc = new XmlDocument();
            svgDoc.AppendChild(svgDoc.CreateElement("svg"));

            // add a <defs> section containing an arrowhead marker
            var defs = svgDoc.CreateElement("defs");
            defs.InnerXml = @"<marker id=""arrowhead"" viewbox=""0 0 10 10"" refX=""1"" refY=""5"" markerWidth=""6"" markerHeight=""6"" orient=""auto""><path d=""M 0 0 L 10 5 L 0 10 z"" /></marker>
<marker id=""arrowhead_from"" viewbox=""0 0 10 10"" refX=""1"" refY=""5"" markerWidth=""6"" markerHeight=""6"" orient=""auto""><path d=""M 0 0 L 10 5 L 0 10 z"" /></marker>
<marker id=""arrowhead_to"" viewbox=""0 0 10 10"" refX=""1"" refY=""5"" markerWidth=""6"" markerHeight=""6"" orient=""auto""><path d=""M 0 0 L 10 5 L 0 10 z"" /></marker>";
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

            // the elements should be written out here sorted by their directions, such that they come out in a clockwise order (this isn't for the diagram itself, but for the edit groups)

            const float tolerance = 0.01f;
            foreach(var kvp in directions)
            {
                var dir = kvp.Value;

                if (dir.Num > 1)
                {
                    dir.DX /= dir.Num;
                    dir.DY /= dir.Num;
                }
                
                if (Math.Abs(dir.DX) < tolerance && Math.Abs(dir.DY) < tolerance)
                {
                    dir.DX = dir.FirstDX; dir.DY = dir.FirstDY;
                }
            }


            foreach (var dir in directions.Values.OrderBy(dir => dir.Angle))
            {
                Console.WriteLine("{0}, {1}", dir.Name, dir.Angle);
                var magnitude = (float)Math.Sqrt(dir.DX * dir.DX + dir.DY * dir.DY);
                dir.DX /= magnitude; dir.DY /= magnitude;

                var line = svgDoc.CreateElement("line");
                svgDoc.DocumentElement.AppendChild(line);

                attr = svgDoc.CreateAttribute("x1");
                attr.Value = (dir.DX * 10).ToString("0.#");
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("y1");
                attr.Value = (dir.DY * 10).ToString("0.#");
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("x2");
                attr.Value = (dir.DX * 100).ToString("0.#");
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("y2");
                attr.Value = (dir.DY * 100).ToString("0.#");
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("class");
                attr.Value = "marker";
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("dir");
                attr.Value = dir.Name;
                line.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("marker-end");
                attr.Value = "url(#arrowhead)";
                line.Attributes.Append(attr);
            }

            return svgDoc.OuterXml;
        }

        public string GetRelativeDirs(VariantVersion version)
        {
            StringBuilder sb = new StringBuilder();
            var dirRoot = GetDefinition(version).Dirs;
            if (dirRoot == null)
                return string.Empty;

            foreach (XmlNode relativeNode in dirRoot.SelectNodes("relative"))
            {
                var relDir = relativeNode.Attributes["name"].Value;

                foreach(XmlNode link in relativeNode.ChildNodes)
                    sb.AppendFormat(";{0}:{1}:{2}", link.Attributes["from"].Value, relDir, link.Attributes["to"].Value);
            }

            return sb.ToString();
        }

        public void SaveRelativeDirs(VariantVersion version, string relData)
        {
            var definition = GetDefinition(version);
            var root = definition.Dirs;
            if (root == null)
            {
                root = definition.Dirs = definition.CreateElement("dirs");
            }
            else
            {
                // clear existing relative directions
                var existingDirs = root.SelectNodes("relative");
                foreach (XmlNode dir in existingDirs)
                    dir.ParentNode.RemoveChild(dir);
            }

            var relativeDirData = new SortedList<string, List<Tuple<string, string>>>();

            var relLink = relData.Split(outerSep, StringSplitOptions.RemoveEmptyEntries);
            foreach (string link in relLink)
            {
                var linkParts = link.Split(innerSep, 3);
                if (linkParts.Length != 3)
                    continue;

                string from = linkParts[0];
                string relDir = linkParts[1];
                string to = linkParts[2];
                
                List<Tuple<string, string>> linksForThisDir;
                if (!relativeDirData.TryGetValue(relDir, out linksForThisDir))
                {
                    linksForThisDir = new List<Tuple<string, string>>();
                    relativeDirData.Add(relDir, linksForThisDir);
                }

                linksForThisDir.Add(new Tuple<string, string>(from, to));
            }

            foreach (var kvp in relativeDirData)
            {
                var relDirNode = definition.CreateElement("relative");
                relDirNode.Attributes.Append(definition.CreateAttribute("name", kvp.Key));

                foreach(var link in kvp.Value)
                {
                    var linkNode = definition.CreateElement("link");
                    linkNode.Attributes.Append(definition.CreateAttribute("from", link.Item1));
                    linkNode.Attributes.Append(definition.CreateAttribute("to", link.Item2));
                    relDirNode.AppendChild(linkNode);
                }

                root.PrependChild(relDirNode);
            }
            definition.Dirs = root;
            version.LastModified = DateTime.Now;
            Entities.SaveChanges();
        }

        public IEnumerable<string> ListGlobalDirections(VariantVersion version)
        {
            SortedSet<string> directions = new SortedSet<string>();

            var board = GetDefinition(version).Board;
            foreach (XmlNode fromCell in board.SelectNodes("cell"))
                foreach (XmlNode link in fromCell.ChildNodes)
                {
                    var dir = link.Attributes["dir"].Value;
                    if (!directions.Contains(dir))
                        directions.Add(dir);
                }

            return directions;
        }

        public IEnumerable<string> ListRelativeDirections(VariantVersion version)
        {
            SortedSet<string> directions = new SortedSet<string>();

            var dirRoot = GetDefinition(version).Dirs;
            if (dirRoot == null)
                return new string[0];

            foreach (XmlNode relativeNode in dirRoot.SelectNodes("relative"))
            {
                var dir = relativeNode.Attributes["name"].Value;
                if (!directions.Contains(dir))
                    directions.Add(dir);
            }

            return directions;
        }

        public string GetDirectionGroups(VariantVersion version)
        {
            var definition = GetDefinition(version);
            var groups = definition.Dirs.SelectNodes("group");

            var sb = new StringBuilder();
            foreach (XmlNode groupNode in groups)
            {
                sb.Append(outerSep);
                sb.Append(groupNode.Attributes["name"].Value);

                foreach (XmlNode dir in groupNode.ChildNodes)
                {
                    sb.Append(innerSep);
                    sb.Append(dir.Attributes["dir"].Value);
                }
            }
            return sb.ToString();
        }

        public void SaveDirectionGroups(VariantVersion version, string groupData)
        {
            var definition = GetDefinition(version);
            var root = definition.Dirs;
            if (root == null)
            {
                root = definition.Dirs = definition.CreateElement("dirs");
            }
            else
            {
                // clear existing direction groups
                var existingGroups = root.SelectNodes("group");
                foreach (XmlNode group in existingGroups)
                    group.ParentNode.RemoveChild(group);
            }

            var groups = groupData.Split(outerSep, StringSplitOptions.RemoveEmptyEntries);
            foreach (string group in groups)
            {
                var groupParts = group.Split(innerSep);

                var groupNode = definition.CreateElement("group");
                groupNode.Attributes.Append(definition.CreateAttribute("name", groupParts[0]));

                for (int i = 1; i < groupParts.Length; i++)
                {
                    var linkNode = definition.CreateElement("include");
                    linkNode.Attributes.Append(definition.CreateAttribute("dir", groupParts[i]));
                    groupNode.AppendChild(linkNode);
                }
                root.AppendChild(groupNode);
            }
            definition.Dirs = root;
            version.LastModified = DateTime.Now;
            Entities.SaveChanges();
        }

        public void SaveCellReferenceChanges(VariantVersion version, string renameData)
        {
            var definition = GetDefinition(version);
            var board = definition.Board;

            var renames = renameData.Split(outerSep, StringSplitOptions.RemoveEmptyEntries);
            foreach (var rename in renames)
            {
                var parts = rename.Split(innerSep);
                if (parts.Length != 2)
                    continue;

                var from = parts[0];
                var to = parts[1];

                var alreadyExists = board.SelectSingleNode(string.Format("cell[@id='{0}']", to));
                if (alreadyExists != null)
                    continue;

                // find the "from" cell, and change its ID to the "to" value.
                var cell = board.SelectSingleNode(string.Format("cell[@id='{0}']", from));
                if (cell == null)
                    continue;
                cell.Attributes["id"].Value = to;

                // do the same for ALL links to this cell
                var links = board.SelectNodes(string.Format("cell/link[@to='{0}']", from));
                foreach (XmlNode link in links)
                    link.Attributes["to"].Value = to;
            }

            definition.Board = board;
            version.LastModified = DateTime.Now;
            Entities.SaveChanges();
        }

        public XmlNode GetPieceDefinitionXML(VariantVersion version)
        {
            var definition = GetDefinition(version);
            return definition.Pieces;
        }

        public bool ValidateXml(string data, out string errors)
        {
            StringBuilder sb = new StringBuilder();
            bool success = true;
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.LoadXml(data);

                var schemas = new XmlSchemaSet();
                using (Stream stream = GetType().Assembly.GetManifestResourceStream("ChessMaker.schema.xsd"))
                {
                    using (XmlReader reader = XmlReader.Create(stream))
                    {
                        schemas.Add("", reader);
                        doc.Schemas = schemas;
                        doc.Validate((o, e) =>
                        {
                            success = false;
                            sb.AppendLine(" ");
                            sb.Append(e.Message);
                            //sb.AppendFormat("Line {1} col {2}: {0}", e.Message, e.Exception.LineNumber, e.Exception.LinePosition); // alas, line numbers ain't working.
                        });
                    }
                }
            }
            catch (Exception e)
            {
                sb.AppendFormat("{0}", e.Message);
                return false;
            }
            finally
            {
                errors = sb.ToString();
            }

            return success;
        }
    }
}