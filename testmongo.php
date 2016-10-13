<?php
date_default_timezone_set('PRC');

define('MGHOST', '127.0.0.1');//mongodb主机名称
define('MGPORT', '27017');//mongodb端口
define('MGDB', 'empery');//mongodb数据库名称
define('MGUSER', '');//mongodb用户名
define('MGPWD', '');//mongodb密码 

define('CPP_DATA_FILE', 'output.txt');
define('QUERY_LOG_FILE', 'query.log');


run();


//执行模拟查询
function run() {
    
    $cpp_data = file(CPP_DATA_FILE);

    $mg = new MongoClient(sprintf("mongodb://%s:%d", MGHOST, MGPORT));
    $db = $mg->selectDB(MGDB);//自动选择数据库
    //$tb = $db->selectCollection('account');

    $totalTime = 0.0;
    $incPos = 0;
    $readNum = 0; $readTime = 0.0;
    $writeNum = 0; $writeTime = 0.0; $writeErrorNum = 0;
    foreach ($cpp_data as $query) {
        preg_match("/collection_name = (.*?), typeid = (.*?), query = (.*?)\#src/", $query, $out);
        if (isset($out[3]) && !empty($out[1])) {

            $operation = $out[2];
            $tableName = $out[1];
            $params = $out[3];

            $json = json_decode($params, true);
            $rt = curtime(); //开始执行的时间格式化
            $st = microtime(true);
            $tb = $db->selectCollection($tableName);
            if (stripos($operation, 'SaveOperation')) {//写操作
                try{
                    $tb->save($json);
                    $str = sprintf('db.%s.save(%s)', $tableName, $params);
                } catch (Exception $ex) {
                    $str = sprintf('db.%s.save(%s), -> [error]:%s', $tableName, $params, $ex->getMessage());
                    $writeErrorNum++;
                }
                
                $et = microtime(true) - $st;
                
                $writeNum++;
                $writeTime += $et;
            } else {//读操作
                $tb->find($json);
                $count = $tb->count($json);
                $et = microtime(true) - $st;
                $str = sprintf('db.%s.find(%s), return count : %d rows', $tableName, $params, $count);
                $readNum++;
                $readTime += $et;
            }            

            $line = sprintf("%s[%f] , %s \n", $rt, $et, $str);
            file_put_contents(QUERY_LOG_FILE, $line, FILE_APPEND);
            echo $line;
            $totalTime += $et;
            $incPos++;
        }
    }
    
    $line = sprintf("%s all query done. total %d lines had process, total time: %s\n", curtime(), $incPos, $totalTime);
    $line .= sprintf("read data:%d (time: %s), write data: %d (time: %s), write error: %d\n", $readNum, $readTime, $writeNum, $writeTime, $writeErrorNum);

    echo $line;
}

//格式时间
function curtime()
{
    list($usec, $sec) = explode(" ", microtime());
    return date('H:i:s#', $sec) . $usec;
}