#! /usr/bin/env python

# $Id$

""" DataOne bandwidth usage viewer

http://www.vijairaj.5gigs.com/project/d1stat

Retrieve the service records from the accounting site and parse the table to
calculate the bandwidth usage. Some plans have free usage period from 0200 -
0800 hrs, any data downloaded during this time in not accounted.

The results might be slightly inaccurate if a session crosses the boundry b/w
free and paid usage hours, because we assume that data downloaded during this
period is evenly distributed, which might not be true.

DISCLAIMER:
    STANDARD DISCLAIMER APPLIES. THIS SOFTWARE WAS MADE FOR THE AUTHORS
    PERSONAL USE AND IS RELEASED TO THE PUBLIC ONLY BECAUSE IT MIGHT BE USEFUL
    TO THE PUBLIC AS WELL. FEEL FREE TO USE AND DISTRIBUTE BUT AT YOUR OWN
    RISK.
"""

__author__ = "Vijairaj R (vijairaj.r@gmail.com)"
__version__ = "$Revision: 3 $"[11:-2]
__copyright__ = "Copyright (c) 2005 Vijairaj R"


import re
import time
import string
import urllib2
from os import path, environ
from urllib import quote as urlquote
from sgmllib import SGMLParser


# -------------------------------------------------------------------- #

CFG_DATE = "2006-02"    # "yyyy-mm"

CFG_USER_NAME = "username"
CFG_PASSWD    = "password"

CFG_STORE_RECORDS = 1
CFG_DBG = 0

# -------------------------------------------------------------------- #


CFG_URL = "http://10.240.0.195"

CFG_URL_LOGIN  = CFG_URL + "/secu/myportallogin.jsp"
CFG_LOGIN_DATA = ( "username=" + CFG_USER_NAME + "&"
                 + "password=" + CFG_PASSWD )

CFG_URL_RECORD = ( CFG_URL
                 + "/myportal/serviceRecords.jsp?"
                 + urlquote(time.strftime("%a, %d %b %Y %H:%M:%S +0000",
                              time.gmtime()), ',:+'))

CFG_URL_SERVICE_RESULTS = CFG_URL + "/myportal/serviceResults.jsp"
CFG_SERVICE_REQ_DATA    = ( "username=" + CFG_USER_NAME + "&"
                          + "queryDate=" + CFG_DATE + "&"
                          + "idAndName=&ispId=" )
CFG_SERVICE_NEXT_PAGE   = "?pageIndex=%d&"

CFG_UA = "MSIE 6.0"

( id_sname, id_startt, id_endt, id_sendkb, id_rcvkb, id_totkb, id_tots,
  id_bsnl_paidkb, id_frees, id_paids ) = range(0, 10)


# -------------------------------------------------------------------- #


class RecordParser(SGMLParser):
    """ Parse the HTML File and get the required info

    Read the information from the html file.
    We are interested only in TABLE 5 which contains the records.
    The First 2 TRs are headings and not needed.

    There is a bug in the HTML file - table4 doesnot end properly.
    """

    def reinit(self):
        self.table_c = 0
        self.tr_c    = 0
        self.td      = False

        self.new_links = []
        self.row = []

    def reset(self):
        SGMLParser.reset(self)

        self.is_valid_record = lambda: (self.table_c == 5 and self.tr_c > 2)

        self.reinit()

        self.table = []

        environ["TZ"] = "GMT" # Avoid complexity
        time.tzset()

    def feed(self, data):
        self.reinit()
        SGMLParser.feed(self, data)
        SGMLParser.close(self)  # Flush the data stream

    def start_a(self, attr):
        self.new_links.append(attr[0][1])

    def start_table(self, attr):
        self.table_c += 1

    def start_tr(self, attr):
        if self.table_c == 5: self.tr_c += 1

    def end_tr(self):
        if self.is_valid_record():
            (free, paid) = self.estimate(self.row[id_startt], self.row[id_endt])
            self.row.append(free) # Col 7, id_freekb
            self.row.append(paid) # Col 8, id_paidkb
            self.table.append(self.row)
            self.row = []

    def start_td(self, attr):
        self.td = True

    def end_td(self):
        self.td = False

    def end_table(self):
        if self.table_c == 5: self.table_c -= 1

    def handle_data(self, data):
        if (self.is_valid_record() and self.td):
            self.row.append(data)

    def estimate(self, str_start, str_end):
        """ Calculate the amount of free & paid secs within a range

        We slow down the clock by 2hrs to ease the calculation.
        """

        d1 = (6 * 3600)         # Free time (0000 --> 0600)
        d2 = (24 * 3600) - d1   # Paid time (0600 --> 2400)

        free = paid = 0

        start_time = ( time.mktime(time.strptime(str_start,'%Y/%m/%d %H:%M:%S'))
                     - (2 * 3600))

        end_time = ( time.mktime(time.strptime(str_end, '%Y/%m/%d %H:%M:%S'))
                   - (2 * 3600))

        i = start_time - start_time % (24 * 3600)

        while (i < end_time):
            i += d1
            if (end_time < i):
                free += (end_time - start_time)
                break
            elif (i > start_time):
                free += (i - start_time)
                start_time = i

            i += d2
            if (end_time < i):
                paid += (end_time - start_time)
            else:
                paid += (i - start_time)
                start_time = i

        return (free, paid)



def GetRecords():
    """ Read the service records and feed it to the parser, finally return the
    service record table as a list.

    This method is highly specific and depends on the site-map of the accounting
    site as on date. If theere is any change in that site, suitable
    modifications should be incorprated here.
    """

    http_handler = urllib2.HTTPHandler(debuglevel = CFG_DBG)
    cookie_handler = urllib2.HTTPCookieProcessor() # Auto cookie handling
    handlers = (http_handler, cookie_handler)

    opener = urllib2.build_opener(*handlers)
    urllib2.install_opener(opener)

    # Get the Login page
    f = urllib2.urlopen(CFG_URL)
    f.read()
    f.close()

    # Post User name and password
    f = urllib2.urlopen(CFG_URL_LOGIN, CFG_LOGIN_DATA)
    data = f.read()
    f.close()

    # Javascript redirection, get the new URL
    next_url = re.findall("http:\/\/[^\"\']*", data)[0]

    # Go to the post login page
    f = urllib2.urlopen(next_url)
    f.read()
    f.close()

    # Go to the service record page
    f = urllib2.urlopen(CFG_URL_RECORD)
    f.read()
    f.close()

    # Read the first page
    f = urllib2.urlopen(CFG_URL_SERVICE_RESULTS, CFG_SERVICE_REQ_DATA)
    data = f.read()
    f.close()

    pageIndex = 1
    parser = RecordParser()

    # Read the rest of the pages and feed it to the parser
    while True:
        parser.feed(data)

        pageIndex += 1
        nextpage = CFG_SERVICE_NEXT_PAGE % pageIndex

        if not path.basename(CFG_URL_SERVICE_RESULTS) + nextpage \
           in parser.new_links:
            break

        f = urllib2.urlopen(CFG_URL_SERVICE_RESULTS + nextpage)
        data = f.read()
        f.close()

    return parser.table



def main():

    table = GetRecords()

    freekb = paidkb = bsnl_paidkb = 0
    freekb_tot = paidkb_tot = 0

    print "Start time\t\t", "End time\t\t", "Total (KB)\t", \
          "Free (KB)\t", "Paid (KB)\t", "Paid (KB) - BSNL"

    while table:
        row = table.pop()

        bsnl_paidkb += int(row[id_bsnl_paidkb])
        freekb = int(int(row[id_totkb]) * row[id_frees] / int(row[id_tots]))
        paidkb = int(int(row[id_totkb]) * row[id_paids] / int(row[id_tots]))

        print row[id_startt], "\t", row[id_endt], "\t", \
              row[id_totkb], "\t\t", freekb, "\t\t", paidkb, \
              "\t\t", row[id_bsnl_paidkb]

        freekb_tot += freekb
        paidkb_tot += paidkb

    print "\n\tUnaccounted %d MB, Accounted %d MB as calulated by d1stat\n" \
          % (freekb_tot/1024, paidkb_tot/1024)

    print "\tTotal accounted usage as per BSNL: %d MB " \
          "<-- this is what you are looking for\n" \
          % (bsnl_paidkb/1024)



if __name__ == "__main__":
    main()


# vim: expandtab smarttab : ts=4 sw=4

