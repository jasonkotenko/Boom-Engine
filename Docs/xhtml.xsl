<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" version="1.0" encoding="UTF-8" indent="yes"/>

<xsl:template match="/">
	<html xml:lang="en" xmlns="http://www.w3.org/1999/xhtml">
		<head>
			<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
			<title><xsl:value-of select="/document/title"/></title>
		</head>
		<body>
			<h1><xsl:value-of select="/document/title"/></h1>
			<p class="author">
				Written by <xsl:value-of select="/document/author"/>
			</p>
			<xsl:for-each select="/document/section">
				<xsl:apply-templates select="."/>
			</xsl:for-each>
		</body>
	</html>
</xsl:template>

<xsl:template match="section">
	<div class="section">
		<xsl:apply-templates/>
	</div>
</xsl:template>

<xsl:template match="heading">
	<h2><xsl:value-of select="."/></h2>
</xsl:template>

<xsl:template match="subheading">
	<h3><xsl:value-of select="."/></h3>
</xsl:template>

<xsl:template match="p">
	<p>
		<xsl:apply-templates/>
	</p>
</xsl:template>

<xsl:template match="strong">
	<strong><xsl:apply-templates/></strong>
</xsl:template>

<xsl:template match="def">
	<span class="def"><strong><xsl:value-of select="./@term"/>:</strong> <span class="desc"><xsl:value-of select="."/></span></span>
</xsl:template>

<xsl:template match="list">
	<ul>
	<xsl:for-each select="./*">
		<li>
			<xsl:apply-templates select="."/>
		</li>
	</xsl:for-each>
	</ul>
</xsl:template>

<xsl:template match="img">
	<img><xsl:attribute name="src"><xsl:value-of select="."/></xsl:attribute></img>
</xsl:template>

</xsl:stylesheet>
