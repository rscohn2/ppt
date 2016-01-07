import json

cg = {}

def read(filename):
    with open(filename,'r') as fp:
        j = json.load(fp)
    for n in j['nodes']:
        # json represents tuples as keys, convert them back to tuples
        n['key'] = tuple(n['key'])
        n['dsts'] = [tuple(x) for x in n['dsts']]
        cg[n['key']] = n

def module(node):
    return node['key'][1]

def name(node):
    return node['key'][0]

def modtable(g):
    mtable = {}
    for k in cg:
        m = k[1]
        if m not in mtable:
            mtable[m] = [cg[k]]
        else:
            mtable[m].append(cg[k])
    for mod in mtable:
        print(mod)
        for n in mtable[mod]:
            print('  ' + name(n))
            for f in n['funcs']:
                print('    ' + f[0] + ':' + f[1])
        
def propset(g):
    for k in cg:
        cg[k]['funcs'] = set()
    changed = True
    while changed:
        changed = False
        for k in cg:
            n = cg[k]
            f = n['funcs']
            for dst in n['dsts']:
                if cg[dst]['native']:
                    f.add(dst)
                else:
                    f |= cg[dst]['funcs']
            if f != n['funcs']:
                n['funcs'] = funcs
                changed = True
                
    
def view():
    g = read('ppt.cg.out')
    propset(g)
    modtable(g)

view()
