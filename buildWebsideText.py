import requests
from bs4 import BeautifulSoup
import time
import datetime

# 处理html
def get_html(url):
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36'}
    return requests.get(url, headers=headers).text


def gen_dates(b_date, days):
    day = datetime.timedelta(days = 1)
    for i in range(days):
        yield b_date + day * i

# 年月日处理函数,返回开始时间和结束时间之间的日期列表
def getdate(BeginDate, EndDate):
    start = datetime.datetime.strptime(BeginDate, "%Y%m%d")
    end = datetime.datetime.strptime(EndDate, "%Y%m%d")
    res = []
    for d in gen_dates(start, (end - start).days):
        res.append(d)
        #print(d)
    return res

# 得到所有底层处理页面的网址
def get_urlList(date):
    baseurl = "http://paper.people.com.cn/rmrb/html/"
    urlList = []
    for d in date:
        year = str(d.year)
        month = str(d.month) if d.month >= 10 else '0' + str(d.month)
        day = str(d.day) if d.day >= 10 else '0' + str(d.day)

        topurl = baseurl + year + '-' + month + '/' + day + '/' + 'nbs.D110000renmrb_01.htm'
        soup = BeautifulSoup(get_html(topurl), 'html.parser') #对每个顶层网址处理，得到多个子网址
        # topDiv  = soup.find_all('div', class_='news')
        # suburl = [a.find("a")["href"] for a in topDiv] # 只能返回第一个href
        suburl = []
        s = soup.find(class_='news-list').find_all('li')
        for tag in s:
            i = str(tag.find('a')['href'])
            suburl.append(i)
        #print(suburl)


        for suburl in suburl :
            suburl = baseurl + year + '-' + month + '/' + day + '/' + suburl
            #print(suburl)
            urlList.append(suburl)
    #print(urlList)
    return urlList

def getans(allurl):
    for sum, url in zip(range(1,5000), allurl):
        soup = BeautifulSoup(get_html(url), 'html.parser')

        title = []
        t = soup.find(class_='article').find_all('h1')
        for tag in t:
            i = str(tag.text)
            title = i
        #print(title)

        infomation = []
        info = soup.find(id='ozoom').find_all('p')
        for tag in info:
            i = str(tag.text)
            infomation.append(i)
        #print(info)

        url = str(url)
        title = str(title)
        data = '#' + str(sum)  + '\n' + url + '\n' + title  + '\n'
        for t in infomation:
            data = data + str(t) + '\n'
        data = data + '|' + '\n'
        #print(data)
        f.writelines(data + "\n")

if __name__ == '__main__':
    BeginDate = input("开始年月日")
    EndDate = input(("结束年月日"))
    datelist = getdate(BeginDate, EndDate)

    filename = "List.txt"
    f = open(filename, 'w', encoding="utf-8")
    allurl = get_urlList(datelist)
    getans(allurl)
    f.close()
    print("保存成功 ")


