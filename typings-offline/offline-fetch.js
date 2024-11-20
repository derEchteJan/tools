
//
// Can be included to replace fetch() to work offline (with strict cors).
// Files are expected to be placed in the dom as a script with the id matching the file path
// and the contents inside.
// For example: <script type="application/json" id="themes/theme-list.json"> ...
// 

class OFResponseText
{
	constructor(body)
	{
		this.body = body
	}
	
	then(mapper)
	{
		if(!this.body) return this;
		mapper(this.body);
		return this;
	}
	
	catch(mapper)
	{
		return this;
	}
}

class OFResponse
{
	constructor(body)
	{
		this.responseText = new OFResponseText(body);
		//console.log("loaded: " + body);
		this.status = 200;
	}
	
	json()
	{
		return JSON.parse(this.text().body);
	}
	
	text()
	{
		return this.responseText;
	}
}

class OFFetch
{
	constructor(url)
	{
		var embeddedElement = document.getElementById(url)
		if(embeddedElement)
		{
			this.response = new OFResponse(embeddedElement.innerHTML);
		}
		else
		{
			this.response = new OFResponse('{"message": "Not Found"}');
			this.response.status = 404;
		}
	}
	
	then(mapper)
	{
		if(!this.response) return this;
		this.response = mapper(this.response)
		return this;
	}
	
	catch(mapper)
	{
		return this;
	}
}

function fetch(_url) {
	console.log("offline-fetch loading '" + _url + "'");
	if(!wordCount) wordCount = 50;
	return new OFFetch(_url);
}