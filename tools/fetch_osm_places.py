#!/usr/bin/env python3
"""Fetch populated places from Overpass for one country, write TSV."""
import json, sys, urllib.parse, urllib.request, time

iso2 = sys.argv[1].upper()
out_tsv = sys.argv[2]
places_arg = sys.argv[3] if len(sys.argv) > 3 else "city|town|village|hamlet"

query = f"""
[out:json][timeout:600];
area["ISO3166-1"="{iso2}"][admin_level=2]->.a;
node["place"~"^({places_arg})$"](area.a);
out tags center;
"""

url = "https://overpass-api.de/api/interpreter"
data = urllib.parse.urlencode({"data": query}).encode()
print(f"querying overpass for {iso2}...", file=sys.stderr)
t0 = time.time()
req = urllib.request.Request(url, data=data, headers={"User-Agent": "smartmet-library-osm/0.1 (mika.heiskanen@fmi.fi)"})
with urllib.request.urlopen(req, timeout=600) as r:
    payload = json.load(r)
elements = payload.get("elements", [])
print(f"  {len(elements)} elements in {time.time()-t0:.1f}s", file=sys.stderr)

cols = ["osm_id","place","name","name_en","iso2","lon","lat","population","timezone","admin1"]
with open(out_tsv, "w", encoding="utf-8") as f:
    f.write("\t".join(cols) + "\n")
    n = 0
    for e in elements:
        t = e.get("tags") or {}
        name = t.get("name") or ""
        if not name: continue
        place = t.get("place","")
        pop_raw = (t.get("population") or "").replace(",","").replace(" ","").strip()
        try:
            pop = int(pop_raw) if pop_raw else ""
        except ValueError:
            pop = ""
        row = [
            str(e.get("id","")),
            place,
            name,
            t.get("name:en",""),
            iso2,
            f"{e.get('lon',''):.6f}" if isinstance(e.get('lon'), (int,float)) else "",
            f"{e.get('lat',''):.6f}" if isinstance(e.get('lat'), (int,float)) else "",
            str(pop),
            t.get("timezone",""),
            t.get("is_in:state", t.get("is_in:region","")),
        ]
        f.write("\t".join(s.replace("\t"," ").replace("\n"," ") for s in row) + "\n")
        n += 1
print(f"  wrote {n} rows -> {out_tsv}", file=sys.stderr)
