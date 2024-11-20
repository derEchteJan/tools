
// declarations

//var results = [];
//var groupedResults = [];
var combined = [];

var categoryDefinitions = [
	{name: "Warframes",   identifier: "category warframe"},
	{name: "Primaries",   identifier: "category primary"},
	{name: "Secondaries", identifier: "category secondary"},
	{name: "Melees",      identifier: "category melee"}
]

var variantDefinitions = [
	{name: "prime",    order: 3},
	{name: "tenet",    order: 3},
	{name: "kuva",     order: 3},
	
	{name: "umbra",    order: 2},
	{name: "wraith",   order: 2},
	{name: "vandal",   order: 2},
	{name: "prisma",   order: 2},
	
	{name: "dex",      order: 1},
	{name: "secura",   order: 1},
	{name: "rakta",    order: 1},
	{name: "telos",    order: 1},
	{name: "sancti",   order: 1},
	{name: "synoid",   order: 1},
	{name: "vaykor",   order: 1},
	{name: "carmine",  order: 1},
	
	{name: "mk1-",     order:-1},
];

StatCategory = class
{
	constructor()
	{
		this.name = "";
		this.ungrouped = [/*StatSingle*/];
		this.grouped = [/*StatGroup*/];
	}
}

StatSingle = class
{
	constructor()
	{
		this.name = "";
		this.value = "";
	}
}

StatVariant = class
{
	constructor()
	{
		this.name = "";
		this.value = 0.0;
		this.order = 0;
	}
}

StatGroup = class
{
	constructor()
	{
		this.name = "";
		this.variants = [/*StatVariant*/];
		this.value = 0.0;
	}
}

function sortStatSingleDesc(lhs, rhs)
{
	if(lhs.value < rhs.value) return 1;
	if(lhs.value > rhs.value) return -1;
	return 0;
}

function sortStatGroupDesc(lhs, rhs)
{
	if(lhs.value < rhs.value) return 1;
	if(lhs.value > rhs.value) return -1;
	return 0;
}

function sortStatVariantAsc(lhs, rhs)
{
	if(lhs.order < rhs.order) return -1;
	if(lhs.order > rhs.order) return 1;
	return 0;
}


// parse ungrouped results

for(categoryDefinition of categoryDefinitions)
{
	var newCategory = new StatCategory();
	newCategory.name = categoryDefinition.name;
	
	var categoryParent = document.getElementsByClassName(categoryDefinition.identifier)[0];
	
	//var yaxisLabelObjects = document.getElementsByClassName('apexcharts-yaxis-label');
	//var dataLabelObjects = document.getElementsByClassName('apexcharts-datalabel');
	var yaxisLabelObjects = categoryParent.querySelectorAll('.apexcharts-yaxis-label');
	var dataLabelObjects = categoryParent.querySelectorAll('.apexcharts-datalabel');

	if(typeof(yaxisLabelObjects) == 'undefined' || yaxisLabelObjects == null) console.error("yaxisLabelObjects is null");
	if(typeof(dataLabelObjects) == 'undefined' || dataLabelObjects == null) console.error("dataLabelObjects is null");
	if(yaxisLabelObjects.length != dataLabelObjects.length) console.error("axis / data objects count mismatch");
	
	for(let i = 0; i < yaxisLabelObjects.length; i++) 
	{
		var yaxisRaw = yaxisLabelObjects[i].firstChild.innerHTML;
		var valueRaw = dataLabelObjects[i].innerHTML;
		var newStat = new StatSingle();
		newStat.name = yaxisRaw.toLowerCase();
		newStat.value = parseFloat(valueRaw.replace(/[^\d.-]/g, ''));
		newCategory.ungrouped.push(newStat);
	}
	
	newCategory.ungrouped.sort(sortStatSingleDesc);
	
	for(let result of newCategory.ungrouped)
	{
		var groupName = "";
		var variant = new StatVariant();
		var variantDefinition = variantDefinitions.find(varDef => result.name.includes(varDef.name));
		if(variantDefinition)
		{
			groupName = result.name.replace(variantDefinition.name, '').trim();
			variant.name  = variantDefinition.name;
			variant.order = variantDefinition.order;
			variant.value = result.value;
		}
		else
		{
			groupName = result.name;
			variant.name  = "none";
			variant.order = 0;
			variant.value = result.value;
		}
		
		let existingGroup = newCategory.grouped.find(group => group.name === groupName)
		if(existingGroup)
		{
			existingGroup.variants.push(variant);
			existingGroup.value = Number((existingGroup.value + variant.value).toFixed(2));
		}
		else
		{
			var newGroup = new StatGroup();
			newGroup.name = groupName;
			newGroup.variants.push(variant);
			newGroup.value = variant.value;
			newCategory.grouped.push(newGroup);
		}
	}
	
	newCategory.grouped.sort(sortStatGroupDesc);
	for(let statGroup of newCategory.grouped)
	{
		statGroup.variants.sort(sortStatVariantAsc);
	}
	
	//console.log("adding category " + newCategory.name);
	combined.push(newCategory);
}

console.log("combined:");
combined