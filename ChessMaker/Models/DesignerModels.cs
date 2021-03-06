﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Xml;

namespace ChessMaker.Models
{
    public abstract class DesignerModel
    {
        public string ThisPage { get; set; }
        public string PrevPage { get; set; }
        public string NextPage { get; set; }

        public string PrevPageTooltip { get; set; }
        public string NextPageTooltip { get; set; }

        public string VariantName { get; set; }
    }

    public class RawModel : DesignerModel
    {
        public RawModel(VariantVersion version)
        {
            VariantName = version.Variant.Name;
            XmlData = version.Definition;
            ThisPage = "Raw xml";
        }

        public string XmlData { get; set; }
    }

    public class BoardShapeModel : DesignerModel
    {
        public BoardShapeModel(VariantVersion version, XmlDocument boardSvg, string linkData)
        {
            VariantName = version.Variant.Name;
            SvgData = boardSvg.OuterXml;
            LinkData = linkData;

            ThisPage = "Shape";
            NextPage = "Global dirs";

            NextPageTooltip = "links between cells";
        }

        public string SvgData { get; set; }
        public string LinkData { get; set; }
    }

    public class GlobalDirectionsModel : DesignerModel
    {
        public GlobalDirectionsModel(VariantVersion version, XmlDocument boardSvg, string linkData)
        {
            VariantName = version.Variant.Name;
            SvgData = boardSvg.OuterXml;
            LinkData = linkData;

            PrevPage = "Shape";
            ThisPage = "Global dirs";
            NextPage = "Relative dirs";

            PrevPageTooltip = "board layout";
            NextPageTooltip = "relative directions";
        }

        public string SvgData { get; set; }
        public string LinkData { get; set; }
    }

    public class RelativeDirectionsModel : DesignerModel
    {
        public RelativeDirectionsModel(VariantVersion version, string globalDirs, string relativeLinks)
        {
            VariantName = version.Variant.Name;
            GlobalDirectionsDiagram = globalDirs;
            RelativeDirections = relativeLinks;

            PrevPage = "Global dirs";
            ThisPage = "Relative dirs";
            NextPage = "Dir groups";

            PrevPageTooltip = "links between cells";
            NextPageTooltip = "grouping directions together";
        }

        public string GlobalDirectionsDiagram { get; set; }
        public string RelativeDirections { get; set; }
    }

    public class DirectionGroupsModel : DesignerModel
    {
        public DirectionGroupsModel(VariantVersion version, string allDirs, string groupData)
        {
            VariantName = version.Variant.Name;
            AllDirs = allDirs;
            GroupData = groupData;

            PrevPage = "Relative dirs";
            ThisPage = "Dir groups";
            NextPage = "Cell refs";

            PrevPageTooltip = "relative directions";
            NextPageTooltip = "unique references for each cell";
        }

        public string AllDirs { get; set; }
        public string GroupData { get; set; }
    }

    public class CellReferencesModel : DesignerModel
    {
        public CellReferencesModel(VariantVersion version, XmlDocument boardSvg, string links, IEnumerable<string> dirs)
        {
            VariantName = version.Variant.Name;
            SvgData = boardSvg.OuterXml;
            LinkData = links;
            AllDirs = dirs;

            PrevPage = "Dir groups";
            ThisPage = "Cell refs";
            NextPage = "Piece types";

            PrevPageTooltip = "grouping directions together";
            NextPageTooltip = "Piece definitions";
        }

        public string SvgData { get; set; }
        public string LinkData { get; set; }
        public IEnumerable<string> AllDirs { get; set; }
    }

    public class PieceDefinitionsModel : DesignerModel
    {
        public PieceDefinitionsModel(VariantVersion version, XmlNode pieceData)
        {
            VariantName = version.Variant.Name;
            PieceData = pieceData != null ? pieceData.OuterXml : "<Pieces />";

            PrevPage = "Cell refs";
            ThisPage = "Piece types";
            NextPage = "Initial layout";

            PrevPageTooltip = "unique references for each cell";
            NextPageTooltip = "Piece starting positions";
        }

        public string PieceData { get; set; }
    }
}