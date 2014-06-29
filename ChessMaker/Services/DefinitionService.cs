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
                // nodes must have the "cell" class to count as a cell
                var classes = node.Attributes["class"];
                if (classes == null)
                    continue;
                var strClasses = " " + classes.Value + " ";
                if (!strClasses.Contains(" cell "))
                    continue;

                var cell = definition.CreateElement("cell");
                
                var id = node.Attributes["id"];
                if (id != null)
                    cell.Attributes.Append(definition.CreateAttribute("id", id.Value));

                var color = strClasses.Contains(" light ") ? "light" : strClasses.Contains(" dark ") ? "dark" : "mid";
                cell.Attributes.Append(definition.CreateAttribute("color", color));

                var path = node.Attributes["d"];
                if (path != null)
                    cell.Attributes.Append(definition.CreateAttribute("path", path.Value));

                // would be neater to "apply" the transform onto the path, so it doesn't need saved/loaded.
                var transform = node.Attributes["transform"];
                if (transform != null)
                    cell.Attributes.Append(definition.CreateAttribute("transform", transform.Value));

                board.AppendChild(cell);
            }
            
            definition.Board = board;
            Entities.SaveChanges();
            return true;
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
                var cell = svgDoc.CreateElement("path");
                svgDoc.DocumentElement.AppendChild(cell);

                attr = svgDoc.CreateAttribute("id");
                attr.Value = node.Attributes["id"].Value;
                cell.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("class");
                attr.Value = "cell " + node.Attributes["color"].Value;
                cell.Attributes.Append(attr);

                attr = svgDoc.CreateAttribute("d");
                attr.Value = node.Attributes["path"].Value;
                cell.Attributes.Append(attr);
            }

            return svgDoc;
        }
    }
}