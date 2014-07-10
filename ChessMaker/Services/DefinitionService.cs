using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Linq;
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

        public bool SaveBoardData(VariantVersion version, string boardSVG)
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
            
            definition.Board = board;
            Entities.SaveChanges();
            return true;
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

            if (strClasses.Contains(" strokeLight "))
                color += " strokeLight";
            else if (strClasses.Contains(" strokeMid "))
                color += " strokeMid";
            else if (strClasses.Contains(" strokeDark "))
                color += " strokeDark";

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
            var color = strClasses.Contains(" strokeLight ") ? "strokeLight" : strClasses.Contains(" strokeDark ") ? "strokeDark" : "strokeMid";
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

        public XmlDocument GetBoardSVG(VariantVersion version)
        {
            var board = GetDefinition(version).Board;

            XmlDocument svgDoc = new XmlDocument();
            svgDoc.AppendChild(svgDoc.CreateElement("svg"));

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
            attr.Value = node.Attributes["color"].Value;
            line.Attributes.Append(attr);
        }
    }
}