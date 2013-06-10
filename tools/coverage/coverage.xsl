<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  >
  <xsl:output
    method="xml"
    omit-xml-declaration="yes"
    indent="yes"
    encoding="UTF-8"
    doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
    xmlns="http://www.w3.org/1999/xhtml"
    />

  <xsl:template match="/CoverageReport">
    <html lang="en" xml:lang="en">
      <head>
        <title>Coverage report</title>
        <style type="text/css">
          <xsl:text disable-output-escaping="yes">
            .progressbar .percent.excelent {
              background-color: green;
            }
            #tree .element.good > .name,
            .progressbar .percent.good,
            .demo .good {
              background-color: greenyellow;
            }
            #tree .element.nice > .name,
            .progressbar .percent.nice,
            .demo .nice {
              background-color: yellow;
            }
            #tree .element.bad > .name,
            .progressbar .percent.bad,
            .demo .bad {
              background-color: orange;
            }
            #tree .element.critical > .name,
            .progressbar .percent.critical,
            .demo .critical {
              background-color: orangeRed;
            }
            #tree .element input:checked ~ .children {
              display: block !important;
            }
            #tree .element input ~ .children {
              display: none;
            }

            table, table td {
              border-width: 1px;
              border-style: solid dotted solid dotted;
              border-color: silver;
              border-collapse: collapse;
              padding-left: 0.2em;
              padding-right: 0.2em;
            }
            .progressbar {
              width: 20em;
            }

            .progressbar .percent {
              display: inline-block;
              height: 0.5em;
              vertical-align: middle;
            }
            div.description:nth-of-type(2n+1) {
              background-color: whitesmoke;
            }
            tr:hover {
              background-color: lightyellow;
            }
          </xsl:text>
        </style>
        <meta http-equiv="content-type" content="text/html;charset=utf-8" />
      </head>
      <body>
        <h1>Coverage report</h1>
        <h2>Summary</h2>
        <table class="summary">
          <xsl:apply-templates select=".|*" mode="summary" />
        </table>

        <h2>Legend</h2>
          <p>Colors correspond to:</p>
          <table class="demo">
            <xsl:call-template name="legend-color">
              <xsl:with-param name="coverage">100</xsl:with-param>
              <xsl:with-param name="description">Full coverage</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="legend-color">
              <xsl:with-param name="coverage">75</xsl:with-param>
              <xsl:with-param name="description">
                <xsl:text>75%-99% coverage, nice</xsl:text>
              </xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="legend-color">
              <xsl:with-param name="coverage">50</xsl:with-param>
              <xsl:with-param name="description">Half the way !</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="legend-color">
              <xsl:with-param name="coverage">0</xsl:with-param>
              <xsl:with-param name="description">Forgot something ?</xsl:with-param>
            </xsl:call-template>
          </table>

        <h2>Tree</h2>
        <!-- Warning IE conditional comment. A warning is displayed if using IE,
           as the tree view has been reported not to work on IE7. ("&lt!" ==  "<") -->
        <xsl:comment>
          <xsl:text>[if IE]>
            &lt;span class="warning" style="background-color: red;font-weight:bold;" >
              This tree representation has been reported not to work on IE.
              Please use a navigator that respect standards.
            &lt;/span>
          &lt;![endif]</xsl:text>
        </xsl:comment>
        <p>Check/Uncheck the checkboxes to fold/unfold.</p>
        <ul class="tree" id="tree">
          <xsl:apply-templates select="." mode="tree" />
        </ul>

        <h2>Description</h2>
        <div class="descriptions">
          <xsl:apply-templates select=".|descendant::*" mode="description" />
        </div>
      </body>
    </html>
  </xsl:template>

  <!--##############
    summary templates
    ##############-->
  <xsl:template mode="summary" match="*">
    <tr>
      <td>
        <a href="#description-{generate-id(.)}">
          <xsl:apply-templates select="." mode="summary-name" />
        </a>
      </td>
      <td><xsl:value-of select="@Coverage" /></td>
      <xsl:apply-templates select="." mode="progress-bar" />
    </tr>
  </xsl:template>

  <xsl:template mode="summary-name" match="/*" >
    <xsl:text>Coverage total average</xsl:text>
  </xsl:template>
  <xsl:template mode="summary-name" match="*">
    <xsl:value-of select="name()" />
  </xsl:template>

  <xsl:template name="legend-color">
    <xsl:param name="coverage" />
    <xsl:param name="description" />
    <tr>
      <td class="coverage">
        <xsl:attribute name="class">
          <xsl:call-template name="choose-quality">
            <xsl:with-param name="coverage">
              <xsl:value-of select="$coverage" />
            </xsl:with-param>
          </xsl:call-template>
        </xsl:attribute>
        <xsl:value-of select="$coverage" />
      </td>
      <td class="coverage">
        <xsl:value-of select="$description" />
      </td>
    </tr>
  </xsl:template>

  <!-- quality coverage templates -->
  <xsl:template name="quality">
    <xsl:call-template name="choose-quality">
      <xsl:with-param name="coverage">
        <xsl:value-of select="number(substring-before(@Coverage,'%'))" />
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="choose-quality">
    <xsl:param name="coverage" />
    <xsl:choose>
      <xsl:when test="$coverage >= 100">excelent</xsl:when>
      <xsl:when test="$coverage >= 75">nice</xsl:when>
      <xsl:when test="$coverage >= 50">bad</xsl:when>
      <xsl:when test="$coverage >= 0">critical</xsl:when>
      <xsl:otherwise />
    </xsl:choose>
  </xsl:template>


  <!--##############
    Tree templates
    ##############-->

  <!-- Ignore the CriterionState nodes and childrens
     as it add a huge number of unimportant info-->
  <xsl:template mode="tree" match="CriterionStates" />

  <xsl:template mode="tree" match="*">
    <li id="tree-{generate-id(.)}">

      <xsl:attribute name="class">
        <xsl:text>element </xsl:text>
        <xsl:call-template name="quality" />
      </xsl:attribute>

      <xsl:apply-templates select="." mode="tree-checkbox" />
      <a class="name description" href="#description-{generate-id(.)}">
        <xsl:value-of select="@Name" />
      </a>

      <xsl:if test="node()">
        <ul class="children"><xsl:apply-templates select="*" mode="tree" /></ul>
      </xsl:if>
    </li>
  </xsl:template>

  <xsl:template mode="tree-checkbox" match="ConfigurableDomain | Criterion">
    <input class="description" type="checkbox" />
  </xsl:template>
  <xsl:template mode="tree-checkbox" match="Domains | Criteria">
    <input class="description" type="checkbox" checked="checked" />
  </xsl:template>
  <xsl:template mode="tree-checkbox" match="*" />

  <!--####################
    Description templates
    ####################-->
  <!-- Ignore the CriterionState nodes and childrens
     as it add a huge number of unimportant info-->
  <xsl:template mode="description" match="CriterionStates|CriterionStates//*" />

  <xsl:template mode="description" match="*">
    <div class="description">
      <h3 id="description-{generate-id(.)}">
        <xsl:value-of select="@Name" />
      </h3>
      <p>
        <xsl:text>Path:</xsl:text>
        <xsl:apply-templates select="ancestor::*|." mode="path" />
      </p>
      <a class="name" href="#tree-{generate-id(.)}">
        <xsl:text>Back to tree</xsl:text>
      </a>
      <h4>Detail</h4>
      <table>
        <xsl:if test="parent::*">
          <tr>
            <td>Type</td>
            <td><xsl:value-of select="name()" /></td>
          </tr>
          <tr>
            <td>Parent</td>
            <td>
              <a class="name" href="#description-{generate-id(..)}">
                <xsl:value-of select="../@Name" />
              </a>
              <xsl:text> (</xsl:text>
              <xsl:value-of select="name(..)" />
              <xsl:text>)</xsl:text>
            </td>
          </tr>
        </xsl:if>
        <xsl:apply-templates select="./@*" mode="description" />
      </table>
      <xsl:if test="node()">
        <h4>Children</h4>
        <table>
          <tr>
            <th>Name</th>
            <th>Type</th>
            <th>Coverage</th>
            <th>Nb use</th>
            <th>Coverage progress bar</th>
          </tr>
          <xsl:apply-templates select="*" mode="description-children" />
        </table>
      </xsl:if>
    </div>
  </xsl:template>

  <xsl:template mode="path" match="*">
    <xsl:text> / </xsl:text>
    <a href="#description-{generate-id(.)}"><xsl:value-of select="@Name" /></a>
  </xsl:template>

  <xsl:template match="@*" mode="description">
    <tr>
      <td><xsl:value-of select="name()" /></td>
      <td><xsl:value-of select="." /></td>
    </tr>
  </xsl:template>


  <!-- Ignore the CriterionState nodes as it add a huge number of unimportant info-->
  <xsl:template mode="description-children" match="CriterionStates" />

  <xsl:template mode="description-children" match="*">
    <tr>
      <td><a class="name" href="#description-{generate-id(.)}">
        <xsl:value-of select="@Name" />
      </a></td>
      <td><xsl:value-of select="name()" /></td>
      <xsl:apply-templates select="./@*" mode="description-children" />
      <xsl:apply-templates select="." mode="progress-bar" />
    </tr>
  </xsl:template>

  <xsl:template match="@Name" mode="description-children" />
  <xsl:template match="@*" mode="description-children">
    <td><xsl:value-of select="." /></td>
  </xsl:template>

  <xsl:template mode="progress-bar" match="*">
    <td class="progressbar">
      <span style="width:{@Coverage}">
        <xsl:attribute name="class">
          <xsl:text>percent </xsl:text>
          <xsl:call-template name="quality" />
        </xsl:attribute>
      </span>
    </td>
  </xsl:template>
</xsl:stylesheet>
