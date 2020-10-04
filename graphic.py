import networkx as nx
import matplotlib.pyplot as plt
import sqlite3

def graph():
    conn = sqlite3.connect('ip.db')
    c = conn.cursor()
    G = nx.Graph()
    c.execute('SELECT ip FROM ip;')
    authors = c.fetchall()
    G.add_nodes_from(authors) 


    G.add_node('yahoo.com.tw')
    G.add_node('google.com')
    G.add_edge(1,2)


    nx.draw(G,with_labels=True)
    plt.draw()
    plt.show()

def coauthors(ip):
    c.execute('SELECT dn \
                FROM ip \
                WHERE ip IS ?;', (ip,))
    out = c.fetchall()
    G.add_edges_from(itertools.product(out, out))

    c.execute('SELECT COUNT() FROM ip;')
    papers = c.fetchall()

    for i in range(1, papers[0][0]+1):
        if i % 1000 == 0:
            print('On record:', str(i))
        coauthors(i)

    
