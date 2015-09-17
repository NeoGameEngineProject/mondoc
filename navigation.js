
function getDocHeight(doc)
{
    doc = doc || document;
    // stackoverflow.com/questions/1145850/
    var body = doc.body, html = doc.documentElement;
    var height = Math.max( body.scrollHeight, body.offsetHeight,
        html.clientHeight, html.scrollHeight, html.offsetHeight );
    return height;
}

function setIframeHeight(id)
{
    var ifrm = document.getElementById(id);
    var doc = ifrm.contentDocument? ifrm.contentDocument:
        ifrm.contentWindow.document;

    ifrm.style.visibility = 'hidden';
    ifrm.style.height = "10px"; // reset to minimal height ...
    // IE opt. for bing/msn needs a bit added or scrollbar appears
    ifrm.style.height = getDocHeight( doc ) + 4 + "px";
    ifrm.style.visibility = 'visible';
}

function loadDiv(div, path)
{
    div.innerHTML='<object width=\"100%\" height=\"100%\" type="text/html" data="' + path + '" ></object>';
}

function loadPage(id, name)
{
    console.log(id, name);
    var location = window.location;
    var url = location.protocol + "//" + location.pathname + "?page=" + name;
    history.replaceState({}, "", url);

    loadDiv(document.getElementById(id), name);
}

function onBodyLoad()
{
    var page = document.URL.match(/page=(.+)/);
    //alert(page);

    var div = document.getElementById("content");
    if(page !== null)
        loadDiv(div, page[1]);
    else
        loadDiv(div, div.src);

    //loadDiv(document.getElementById("sidepanel"), "sidepanel.html");
}
