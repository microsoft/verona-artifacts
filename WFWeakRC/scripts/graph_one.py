import plotly.graph_objects as go
import argparse
import sys
import os
import csv
import plotly.express as px
import numpy as np


one_df = []
one_df2 = []
all_df = []
with open("results.csv", newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    for line in reader:
        type, bench, cores, time, iteration = line
        if (int(iteration) < 5):
            continue

        if bench == "one" and int(cores) in {1,4,9,16} and type != "Wait-free (Opt)":
            one_df.append({"HW threads": int(cores), "HW Threads": cores,
                       "time": int(time), "type": type, "bench": bench, "iteration": int(iteration)})
        if bench == "one" and int(cores) in {25,36,49,64} and type != "Wait-free (Opt)":
            one_df2.append({"HW threads": int(cores), "HW Threads": cores,
                       "time": int(time), "type": type, "bench": bench, "iteration": int(iteration)})
        if bench != "one":
            all_df.append({"HW threads": int(cores), "HW Threads": cores,
                       "time": int(time), "type": type, "bench": bench, "iteration": int(iteration)})

print("Loaded data")

layout = go.Layout(
    hovermode='closest',
    legend=dict(
        orientation="v",
        yanchor="top",
        y=.6,
        xanchor="left",
        x=0.111,
        bgcolor="white",
        bordercolor="Black",
        borderwidth=1,
#          itemsizing = "trace",
        entrywidth=10,
    ),
    margin=dict(
        l=0,
        r=0,
        b=0,
        t=15,
    ),
    font=dict(
        family="Time New Roman",
        size=7,
        color="black",
    ),
    yaxis=dict(
        title="",
        ticksuffix="%",
        range=[0, 105],
    ),
    xaxis=dict(
        range=[1.5, 5.5],
        type="log",
    )
)

fig = px.ecdf(one_df, x="time", color="type",
                facet_col="HW threads", ecdfnorm="percent", log_x=True, width=650, height=150,
                labels={
                    "type": "",
                    "time": ""
                }
                )
# Set line type based on type
fig.for_each_trace (lambda t: t.update(line=dict(dash="dot")) if t.name == "Classic" else None)
fig.update_layout(layout)
fig.write_html(f"one.html")
fig.write_image(f"one.pdf")

print ("Done one")

fig = px.ecdf(one_df2, x="time", color="type",
                facet_col="HW threads", ecdfnorm="percent", log_x=True, width=650, height=150,
                labels={
                    "type": "",
                    "time": "Cycles"
                }
                )
# Set line type based on type
fig.for_each_trace (lambda t: t.update(line=dict(dash="dot")) if t.name == "Classic" else None)
fig.update_layout(layout)
# Don't show legend in second graph
fig.update_layout(showlegend=False)
fig.write_html(f"one2.html")
fig.write_image(f"one2.pdf")

print("Done one2")

df = [x for x in all_df if x["bench"] == "upgrade" and x["type"] != "Wait-free (Opt)"]
fig = px.scatter(df, x="HW threads", y="time", color="type",
            labels={
                    "type": "",
                    "time": "Time (milliseconds)",
                    "HW threads": "Hardware Threads"
                }, width=300, height=250)

layout = go.Layout(
    hovermode='closest',
    legend=dict(
        orientation="v",
        yanchor="top",
        y=0.98,
        xanchor="left",
        x=0.01,
        title="",
        bgcolor="white",
        bordercolor="Black",
        borderwidth=1,
        # itemsizing = "constant",
    ),
    margin=dict(
        l=0,
        r=0,
        b=0,
        t=0,
    ),
    font=dict(
        family="Courier New, monospace",
        size=7,
        color="black",
    ),
)
# Specify marker based on type
fig.for_each_trace (lambda t: t.update(marker=dict(opacity=1, size=5, symbol="cross" if t.name == "Classic" else "x" if t.name == "Wait-free" else "circle-cross")))

fig.update_layout(layout)
fig.write_html(f"upgrade.html")
fig.write_image(f"upgrade.pdf")
    
print("Done upgrade")

df = [x for x in all_df if x["bench"] == "strong" and x["type"] != "Wait-free (Opt)"]

fig = px.scatter(df, x="HW threads", y="time", color="type",
                    labels={
                    "type": "",
                    "time": "Time (milliseconds)",
                    "HW threads": "Hardware Threads"
                }, width=300, height=300)

# Specify marker based on type
fig.for_each_trace (lambda t: t.update(line=dict(width=0.1), marker=dict(opacity=1, size=5, symbol="cross" if t.name == "Classic" else "x" if t.name == "Wait-free" else "circle-cross")))


layout = go.Layout(
    hovermode='closest',
    legend=dict(
        orientation="v",
        yanchor="top",
        y=0.95,
        xanchor="left",
        x=0.01,
        bgcolor="White",

        bordercolor="Black",
        borderwidth=1,
        title="",
#          itemsizing = "constant",
    ),
    margin=dict(
        l=0,
        r=0,
        b=0,
        t=0,
    ),
    font=dict(
        family="Courier New, monospace",
        size=7,
        color="black",
    ),
    yaxis=dict(
        range=[0, 40000],
    ),

)

fig.update_layout(layout)
fig.write_html(f"strong.html")
fig.write_image(f"strong.pdf")

print("Done strong")

df = [x for x in all_df if x["bench"] == "opt_bad"]

fig = px.violin(df, x="type", y="time", color="type", points=False,
                    labels={
                    "type": "",
                    "time": "Time (milliseconds)"}, 
                width=300, height=300)

# Specify marker based on type
fig.for_each_trace (lambda t: t.update(line=dict(width=0.1), marker=dict(opacity=1, size=5, symbol="cross" if t.name == "Classic" else "x" if t.name == "Wait-free" else "circle-cross")))


layout = go.Layout(
    hovermode='closest',
    legend=dict(
        orientation="v",
        yanchor="bottom",
        y=0.05,
        xanchor="left",
        x=0.55,
        bgcolor="White",

        bordercolor="Black",
        borderwidth=1,
        title="",
#          itemsizing = "constant",
    ),
    margin=dict(
        l=0,
        r=0,
        b=0,
        t=0,
    ),
    font=dict(
        family="Courier New, monospace",
        size=7,
        color="black",
    ),
    yaxis=dict(
        range=[0, 40000],
    ),

)
fig.update_layout(layout)
fig.update_layout(showlegend=False)
fig.write_html(f"opt_bad.html")
fig.write_image(f"opt_bad.pdf")

print("Done opt_bad")

df = [x for x in all_df if x["bench"] == "strong" and (x["type"] == "Classic" or x["type"] == "Wait-free (Opt)") and x["HW threads"] in [20, 60]]

fig = px.violin(df, x="HW Threads", y="time", color="type", points=False,
                    labels={
                    "type": "",
                    "time": "Time (milliseconds)"}, 
                width=300, height=300)

# Specify marker based on type
fig.for_each_trace (lambda t: t.update(line=dict(width=0.1), marker=dict(opacity=1, size=5, symbol="cross" if t.name == "Classic" else "x" if t.name == "Wait-free" else "circle-cross")))


layout = go.Layout(
    hovermode='closest',
    legend=dict(
        orientation="v",
        yanchor="bottom",
        y=0.05,
        xanchor="left",
        x=0.55,
        bgcolor="White",

        bordercolor="Black",
        borderwidth=1,
        title="",
#          itemsizing = "constant",
    ),
    margin=dict(
        l=0,
        r=0,
        b=0,
        t=0,
    ),
    font=dict(
        family="Courier New, monospace",
        size=7,
        color="black",
    ),
    yaxis=dict(
        range=[0, 20000],
    ),

)
fig.update_layout(layout)
#fig.update_layout(showlegend=False)
fig.write_html(f"opt_good.html")
fig.write_image(f"opt_good.pdf")
