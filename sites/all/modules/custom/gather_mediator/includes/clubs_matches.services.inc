<?php

function gather_mediator_clubs_matches_services_resource() {
    $api = array(
        'clubs_matches' => array(
            'operations' => array(
                'index' => array(
                    'help' => 'Club Listing',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/clubs_matches.services',
                    ),
                    'callback' => 'gather_mediator_clubs_matches_resource_retrieve',
                    'access arguments' => array('access content'),
                    //'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'club_id',
                            'optional' => TRUE,
                            'type' => 'int',
                            'description' => 'The zero-based index of the page to get, defaults to 0.',
                            'default' => '0',
                            'source' => array('param' => 'club_id'),
                        ),
                        array(
                            'name' => 'user_id',
                            'optional' => TRUE,
                            'type' => 'int',
                            'description' => 'The zero-based index of the page to get, defaults to 0.',
                            'default' => '0',
                            'source' => array('param' => 'user_id'),
                        ),
                    ),

                ),
                'create' => array(
                    'help' => 'Club Matches Create',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/clubs_matches.services',
                    ),
                    'callback' => 'gather_mediator_clubs_matches_resource_create',
                    'access callback' => 'clubs_matches_services_resource_access',
                    'access arguments' => array('create'),
                    'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'node',
                            'type' => 'array',
                            'description' => 'create the matches.',
                            'source' => 'data',
                            'optional' => FALSE,
                        ),
                    ),

                ),
                'update' => array(
                    'help' => 'Update a Match',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/clubs_matches.services',
                    ),'callback' => 'gather_mediator_clubs_matches_resource_update',
                    'args' => array(
                        array(
                            'name' => 'nid',
                            'optional' => FALSE,
                            'source' => array('path' => 0),
                            'type' => 'int',
                            'description' => 'The nid of the node to update',
                        ),
                        array(
                            'name' => 'node',
                            'optional' => FALSE,
                            'source' => 'data',
                            'description' => 'The node data to update',
                            'type' => 'array',
                        ),
                    ),
                    'access callback' => 'clubs_matches_services_resource_access',
                    'access arguments' => array('update'),
                    'access arguments append' => TRUE,
                ),

            ),
        ),

    );


    return $api;
}

function gather_mediator_clubs_matches_resource_retrieve($club_id,$user_id){

    global $user;

    $user_load = user_load($user->uid);
    if($user->uid <= 0){
        $error = "Access Denied for anonymous User";
        return services_error(t("Match Error"),406,array("club_match_error"=>$error));
    }

    $data = array();
    if(!empty($club_id)){

        $home_query = new EntityFieldQuery();
        $home_query->entityCondition('entity_type', 'field_collection_item')
            ->entityCondition('bundle', 'field_club_detail')
            ->fieldCondition('field_home_club_id', 'value', $club_id)
            ->propertyOrderBy('item_id','DESC');
        $home_team_result = $home_query->execute();
        if(empty($home_team_result)){
            $home_team_result['field_collection_item'] = array();
        }
        $opp_query = new EntityFieldQuery();
        $opp_query->entityCondition('entity_type', 'field_collection_item')
            ->entityCondition('bundle', 'field_club_detail')
            ->fieldCondition('field_opponent_club_id', 'value', $club_id)
            ->propertyOrderBy('item_id','DESC');
        $opp_team_result = $opp_query->execute();
        if(empty($opp_team_result)){
            $opp_team_result['field_collection_item'] = array();
        }
        $team_result = array_merge($home_team_result['field_collection_item'],$opp_team_result['field_collection_item']);


        $i = 0;
        if(!empty($team_result)) {
            foreach ($team_result as $key => $value) {
                $now = time();
                $date = date("Y-m-d", $now);
                $time1 = new DateTime($date); // string date
                $time2 = new DateTime();
                $node = entity_load_single('field_collection_item', $value->item_id);
                $host_entity = $node->hostEntity();
                $time2->setTimestamp($host_entity->created); // timestamps,  it can be string date too.
                $interval = $time2->diff($time1);
                if ($interval->days <= '7') {
                    $user_load1 = user_load($host_entity->uid);
                    $home_club = node_load($node->field_home_club_id['und'][0]['value']);
                    $opp_club = node_load($node->field_opponent_club_id['und'][0]['value']);
                    $data[$i]['user_id'] = $user_load1->uid;
                    $data[$i]['first_name'] = $user_load1->field_first_name['und'][0]['value'];
                    $data[$i]['last_name'] = $user_load1->field_last_name['und'][0]['value'];
                    $data[$i]['user_image'] = file_create_url($user_load1->field_user_image['und'][0]['uri']);
                    $data[$i]["match_id"] = $host_entity->nid;
                    $data[$i]["match_name"] = $host_entity->title;
                    $data[$i]["match_description"] = $host_entity->field_match_description['und'][0]['value'];

                    $data[$i]["match_status"] = $host_entity->field_match_status['und'][0]['value'];
                    $data[$i]['match_start_date'] = format_date($host_entity->created, $type = 'custom', 'd-m-Y H:m:s');
                    $data[$i]['match_end_date'] = $host_entity->field_match_date['und'][0]['value2'];
                    $data[$i]["match_likes"] = $host_entity->field_match_likes['und'][0]['value'];
                    $data[$i]["home_team"] = $home_club->title;
                    $data[$i]["home_team_image"] = file_create_url($home_club->field_club_image['und'][0]['uri']);
                    $data[$i]["opponent_team"] = $opp_club->title;
                    $data[$i]["opp_team_image"] = file_create_url($opp_club->field_club_image['und'][0]['uri']);
                    $i++;
                }

            }
        }


    }
    else {

        // if(empty($user_id)){
        // $error = "Please Specify the User Id";
        // return services_error(t("Match Error"),406,array("club_match_error"=>$error));
        // }

        if(!empty($user_id) && $user_id > 0 ) {
            // $user_load = user_load($user->uid);
            $user_follow_matches = array();
            if (isset($user_load->field_list_of_clubs['und'])) {
                foreach ($user_load->field_list_of_clubs['und'] as $key => $clubs_ids) {

                    $user_follow_matches[] = $clubs_ids['target_id'];
                }
            }

            if (empty($user_follow_matches)) {
                $error = "User has not followed any club";
                return services_error(t("Match Error"), 406, array("club_match_error" => $error));
            }

            $query3 = new EntityFieldQuery();
            $query3->entityCondition('entity_type', 'field_collection_item')
                ->entityCondition('bundle', 'field_club_detail')
                ->fieldCondition('field_home_club_id', 'value', $user_follow_matches, "IN")
                ->propertyOrderBy('item_id', 'ASC');
            $home_team_result = $query3->execute();
            if (empty($home_team_result)) {
                $home_team_result['field_collection_item'] = array();
            }

            $query4 = new EntityFieldQuery();
            $query4->entityCondition('entity_type', 'field_collection_item')
                ->entityCondition('bundle', 'field_club_detail')
                ->fieldCondition('field_opponent_club_id', 'value', $user_follow_matches, "IN")
                ->propertyOrderBy('item_id', 'ASC');
            $opp_team_result = $query4->execute();
            if (empty($opp_team_result)) {
                $opp_team_result['field_collection_item'] = array();
            }

            $team_result = array_merge($home_team_result['field_collection_item'], $opp_team_result['field_collection_item']);
            usort($team_result, "asortFunction");
            //    print_r($team_result);

            $i = 0;
            $temp_matchId_array = array();
            if (!empty($team_result)) {
                foreach ($team_result as $key => $value) {

                    $now = time();
                    $date = date("Y-m-d", $now);
                    //$date = "2016-07-02";
                    $time1 = new DateTime($date); // string date
                    $time2 = new DateTime();

                    $node = entity_load_single('field_collection_item', $value->item_id);

                    $host_entity = $node->hostEntity();

                    $user_load1 = user_load($host_entity->uid);
                    $host_entity = $node->hostEntity();
                    $time2->setTimestamp($host_entity->created); // timestamps,  it can be string date too.
                    $interval = $time2->diff($time1);
                    usort($host_entity, "sortFunction"); 
                   //  print_r($host_entity);die();
                    /// $interval->days <= '7';
                   // print_r($user_load1->uid);die;
                    if ($interval->days <= '6') {
                        if (!in_array($host_entity->nid, $temp_matchId_array)) {
                            $home_club = node_load($node->field_home_club_id['und'][0]['value']);
                            $opp_club = node_load($node->field_opponent_club_id['und'][0]['value']);
                            $data[$i]['user_id'] = $user_load1->uid;
                            $data[$i]['first_name'] = $user_load1->field_first_name['und'][0]['value'];
                            $data[$i]['last_name'] = $user_load1->field_last_name['und'][0]['value'];
                            $data[$i]['user_image'] = file_create_url($user_load->field_user_image['und'][0]['uri']);
                            $data[$i]["match_id"] = $host_entity->nid;
                            $temp_matchId_array[] = $host_entity->nid;
                            $data[$i]["match_name"] = $host_entity->title;
                            $data[$i]["match_description"] = $host_entity->field_match_description['und'][0]['value'];
                            $data[$i]["match_status"] = $host_entity->field_match_status['und'][0]['value'];
                            //if ($interval->days <= '7') {
                            $data[$i]['match_start_date'] = format_date($host_entity->created, $type = 'custom', 'd-m-Y H:i:s');
                            $data[$i]['match_end_date'] = $host_entity->field_match_date['und'][0]['value2'];
                            $data[$i]["match_likes"] = $host_entity->field_match_likes['und'][0]['value'];
                            $data[$i]["home_team"] = $home_club->title;
                            $data[$i]["home_team_image"] = file_create_url($home_club->field_club_image['und'][0]['uri']);
                            $data[$i]["opponent_team"] = $opp_club->title;
                            $data[$i]["opp_team_image"] = file_create_url($opp_club->field_club_image['und'][0]['uri']);
                            $i++;
                        }

                    }
                }

            }

/*            $query = new EntityFieldQuery();
            $query->entityCondition('entity_type', 'node')
                ->entityCondition('bundle', 'matches')// ex. article
                ->propertyCondition('uid', $user->uid); // node written by a specific user
            $result = $query->execute();
            foreach ($result['node'] as $key => $value) {
                $host_entity1 = node_load($value->nid);
                $now = time();
                $date = date("Y-m-d", $now);
                //$date = "2016-07-02";
                $time1 = new DateTime($date); // string date
                $time2 = new DateTime();
              //print_r($host_entity1->created);die;
                $time2->setTimestamp($host_entity1->created); // timestamps,  it can be string date too.
                $interval = $time2->diff($time1);
                if ($interval->days <= '7') {
                    $value_id = entity_load_single('field_collection_item', $host_entity1->field_club_detail['und'][0]['value']);
                    $node = entity_load_single('field_collection_item', $value_id->item_id);

                    $home_club = node_load($node->field_home_club_id['und'][0]['value']);
                    $opp_club = node_load($node->field_opponent_club_id['und'][0]['value']);
                    $match_user = user_load($host_entity1->uid);
                    $data[$i]['user_id'] = $match_user->uid;
                    $data[$i]['first_name'] = $match_user->field_first_name['und'][0]['value'];
                    $data[$i]['last_name'] = $match_user->field_last_name['und'][0]['value'];
                    $data[$i]['user_image'] = file_create_url($match_user->field_user_image['und'][0]['uri']);
                    $data[$i]["match_id"] = $host_entity1->nid;
                    $data[$i]["match_name"] = $host_entity1->title;
                    $data[$i]["match_description"] = $host_entity1->field_match_description['und'][0]['value'];
                    $data[$i]["match_status"] = $host_entity1->field_match_status['und'][0]['value'];

                    //if ($interval->days <= '7') {
                    $data[$i]['match_start_date'] = format_date($host_entity1->created, $type = 'custom', 'd-m-Y H:i:s');
                    $data[$i]['match_end_date'] = $host_entity1->field_match_date['und'][0]['value2'];
                    $data[$i]["match_likes"] = $host_entity1->field_match_likes['und'][0]['value'];
                    $data[$i]["home_team"] = $home_club->title;
                    $data[$i]["home_team_image"] = file_create_url($home_club->field_club_image['und'][0]['uri']);
                    $data[$i]["opponent_team"] = $opp_club->title;
                    $data[$i]["opp_team_image"] = file_create_url($opp_club->field_club_image['und'][0]['uri']);
                    $i++;
                }
            }*/

        }
        else {

            $query5 = new EntityFieldQuery();
            $entities = $query5->entityCondition('entity_type', 'node')
                ->entityCondition('bundle', "matches")
                ->propertyCondition('uid', $user->uid)
                ->propertyOrderBy('created', 'ASC')
                ->execute();

            if (!empty($entities['node'])) {
                $nodes = node_load_multiple(array_keys($entities['node']));
            }


            if (!empty($nodes)) {
                $i = 0;
                foreach ($nodes as $key => $values) {
                    $now = time();
                    $date = date("Y-m-d", $now);
                    //$date = "2016-07-02";
                    $time1 = new DateTime($date); // string date
                    $time2 = new DateTime();
                    $time2->setTimestamp($values->created); // timestamps,  it can be string date too.
                    $interval = $time2->diff($time1);

                    if ($interval->days <= '6') {
                        // print_r($values);die;
                        $node = entity_load_single('field_collection_item', $values->field_club_detail['und'][0]['value']);
                        // if (!in_array($node->field_home_club_id['und'][0]['value'], $user_follow_matches) || !in_array($node->field_opponent_club_id['und'][0]['value'], $user_follow_matches)){
                        //if (!in_array($node->field_home_club_id['und'][0]['value'], $user_follow_matches)){
                        $home_club = node_load($node->field_home_club_id['und'][0]['value']);
                        $opp_club = node_load($node->field_opponent_club_id['und'][0]['value']);
                        $data[$i]['user_id'] = $user_load->uid;
                        $data[$i]['first_name'] = $user_load->field_first_name['und'][0]['value'];
                        $data[$i]['last_name'] = $user_load->field_last_name['und'][0]['value'];
                        $data[$i]['user_image'] = file_create_url($user_load->field_user_image['und'][0]['uri']);
                        $data[$i]["match_id"] = $key;
                        $data[$i]["match_name"] = $values->title;
                        $data[$i]['match_start_date'] = format_date($values->created, $type = 'custom', 'd-m-Y H:m:s');
                        $data[$i]['match_end_date'] = $values->field_match_date['und'][0]['value2'];
                        $data[$i]["match_likes"] = $values->field_match_likes['und'][0]['value'];
                        $data[$i]["match_status"] = $values->field_match_status['und'][0]['value'];
                        $data[$i]["home_team"] = $home_club->title;
                        $data[$i]["home_team_image"] = file_create_url($home_club->field_club_image['und'][0]['uri']);
                        $data[$i]["opponent_team"] = $opp_club->title;
                        $data[$i]["opp_team_image"] = file_create_url($opp_club->field_club_image['und'][0]['uri']);
                        $i++;
                        //}
                    }

                }

            }
        }
    }

    $match_id = array();
    foreach ($data as $match) {
        $match_id[] = $match['match_id'];
    }

    array_multisort($match_id, SORT_DESC, $data);

    foreach ($data as $key => $match) {
        if($match['match_status'] == 'end'){
           unset($data[$key]);
           array_push($data, $match);
        }
    }
    $data =  array_values($data);
    //print_r($data);die;
    return $data;

}

function compare_match_status($a, $b)
{
 //   return strcmp($a["match_status"], $b["match_status"]);
    if ($a['match_status'] == "end") {

        return True;

    }
    if ($b['match_status'] == "end") {

        return False;

    }
}
function customShift($array){

    foreach($array as $key => $val) {

        if($val['match_status'] == 'live') {

            $item = $array[$key];

            unset($array[$key]);

            array_push($array, $item);
            print_r($array);


        }else{
            print_r('ger');
        }
    }
}
function sortFunction($a,$b){
    if ($a->nid == $b->nid) return 0;
    return $a->nid < $b->nid?1:-1;
}
function asortFunction($a,$b){
    if ($a->item_id == $b->item_id) return 0;
    return $a->item_id < $b->item_id?1:-1;
}
function gather_mediator_clubs_matches_resource_create($node)
{

    global $user;
    $match = new StdClass();
    if(empty($node['title'])){
        $error = "Please Give Match Title";
        return services_error(t("Match Error"),403,array("club_match_error"=>$error));


    }
    if(empty($node['type']) || $node['type'] !== "matches"){
        $error = "Please Specify the Type of the Match";
        return services_error(t("Match Error"),406,array("club_match_error"=>$error));
    }
    if($node['type'] !== "matches"){
        $error = "Please Specify the Type of the Match";
        return services_error(t("Match Error"),406,array("club_match_error"=>$error));
    }
    $match->title = $node['title'];
//creating a bare node
    $match->type = $node['type'];
    node_object_prepare($match);
    $match->language = "und"; // Or e.g. 'en' if locale is enabled
    $match->uid = $user->uid;
    $match->name = $user->name;
    $match->status = 1; //(1 or 0): published or not
    $match->promote = 0; //(1 or 0): promoted to front page
    $match->comment = 1; // 0 = comments disabled, 1 = read only, 2 = read/write

    $match->field_match_date['und'][0]['value'] = $node['field_match_date']['value'];
    $match->field_match_date['und'][0]['value2'] =  $node['field_match_date']['value2'];
    $match->field_match_likes['und'][0]['value'] = $node['field_match_likes'];
    $match->field_match_description['und'][0]['value'] = $node['field_match_description'];
    if(empty($node['field_club_detail'])){
        $error = "Please specify the club details";
        return services_error(t("Match Error"),406,array("club_match_error"=>$error));
    }
    $fc_item = entity_create('field_collection_item', array('field_name' => 'field_club_detail'));
    $fc_item->setHostEntity('node', $match);
    $fc_wrapper = entity_metadata_wrapper('field_collection_item', $fc_item);
    $fc_wrapper->field_home_club_id->set($node['field_club_detail']['field_home_club_id']);
    $fc_wrapper->field_home_club_name->set($node['field_club_detail']['field_home_club_name']);
    $fc_wrapper->field_opponent_club_id->set($node['field_club_detail']['field_opponent_club_id']);
    $fc_wrapper->field_opponent_club_name->set($node['field_club_detail']['field_opponent_club_name']);
    $fc_wrapper->save(TRUE);
    if(empty($node['field_home_team_detail'])){
        $error = "Please specify the home team details";
        return services_error(t("Match Error"),406,array("club_match_error"=>$error));
    }


    if (isset($node['field_home_team_detail']) && !empty($node['field_home_team_detail'])) {
        foreach ($node['field_home_team_detail'] as $key => $home_team_fields) {

            $fc_item2 = entity_create('field_collection_item', array('field_name' => 'field_home_team_detail'));
            $fc_item2->setHostEntity('node', $match);

            $fc_item2->field_tid['und'][]['value'] = $home_team_fields['team_id'];
            $fc_item2->field_tname['und'][]['value'] = $home_team_fields['team_player'];
            $fc_item2->field_tnumber['und'][]['value'] = $home_team_fields['field_team_jersey_number'];

            $fc_item2->save(TRUE);

        }
    }
    if(empty($node['field_opponent_team_detail'])){
        $error = "Please specify the opponent team details";
        return services_error(t("Match Error"),406,array("club_match_error"=>$error));
    }

    if (isset($node['field_opponent_team_detail']) && !empty($node['field_opponent_team_detail'])) {
        foreach ($node['field_opponent_team_detail'] as $key => $home_team_fields) {
            $fc_item3 = entity_create('field_collection_item', array('field_name' => 'field_opponent_team_detail'));
            $fc_item3->setHostEntity('node', $match);

            $fc_item3->field_tid['und'][]['value'] = $home_team_fields['team_id'];
            $fc_item3->field_tname['und'][]['value'] =$home_team_fields['team_player'];
            $fc_item2->field_tnumber['und'][]['value'] = $home_team_fields['field_team_jersey_number'];

            $fc_item3->save(TRUE);

        }
    }


    node_save($match);
    $node_match = node_load($match->nid);

    if($node_match){
        // echo '<pre>';print_r($node_match);die;
        $user_load = user_load($user->uid);
        $data['user_id'] = $user_load->uid;
        $data['first_name'] = $user_load->field_first_name['und'][0]['value'];
        $data['last_name'] = $user_load->field_last_name['und'][0]['value'];
        $data['user_image'] = '';
        if(isset($user_load->field_user_image['und']) && !empty($user_load->field_user_image['und'][0]['uri'])) {
            $data['user_image'] = file_create_url($user_load->field_user_image['und'][0]['uri']);
        }
        $data["match_id"] = $node_match->nid;
        $data["match_name"] = $node_match->title;
        $data['match_start_date'] = $node_match->field_match_date['und'][0]['value'];
        $data['match_end_date'] = $node_match->field_match_date['und'][0]['value2'];
        $data["match_likes"] = $node_match->field_match_likes['und'][0]['value'];
        $data["match_description"] = $node_match->field_match_description['und'][0]['value'];

        $club_entity_details =   entity_load_single('field_collection_item',  $node_match->field_club_detail['und'][0]['value']);

        $home_club = node_load($club_entity_details->field_home_club_id['und'][0]['value']);
        $opp_club = node_load($club_entity_details->field_opponent_club_id['und'][0]['value']);
        $data["home_id"] = $home_club->nid;
        $data["home_team"] = $home_club->title;

        $data['home_team_image'] = '';
        if(isset($home_club->field_club_image['und']) && !empty($home_club->field_club_image['und'][0]['uri'])) {
            $data['home_team_image'] = file_create_url($home_club->field_club_image['und'][0]['uri']);
        }
        $data["opp_id"] = $opp_club->nid;
        $data["opponent_team"] =$opp_club->title;
        $data['opp_team_image'] = '';
        if(isset($opp_club->field_club_image['und']) && !empty($opp_club->field_club_image['und'][0]['uri'])) {
            $data['opp_team_image'] = file_create_url($opp_club->field_club_image['und'][0]['uri']);
        }
        $data['opp_team_points'] = $club_entity_details->field_opponent_club_points['und'][0]['value'];
        $data['opp_team_goals'] = $club_entity_details->field_opponent_club_goals['und'][0]['value'];


    }

    return $data;



}

/**
 * Updates a new node based on submitted values.
 *
 * Note that this function uses drupal_form_submit() to create new nodes,
 * which may require very specific formatting. The full implications of this
 * are beyond the scope of this comment block. The Googles are your friend.
 *
 * @param $nid
 *   Node ID of the node we're editing.
 * @param $node
 *   Array representing the attributes a node edit form would submit.
 * @return
 *   The node's nid.
 *
 * @see drupal_form_submit()
 */
function gather_mediator_clubs_matches_resource_update($nid, $node) {


    $node = _services_arg_value($node, 'node');
    $node['nid'] = $nid;
    $old_node = node_load($nid);
    $old_node->field_match_status['und'][0]['value'] = $node['field_match_status'];
    node_save($old_node);
    return $node;
}


function clubs_matches_services_resource_access($op = 'view', $args = array()) {
    // Adds backwards compatability with regression fixed in #1083242


    if (isset($args[0])) {
        $args[0] = _services_access_value($args[0], array('account', 'data'));
    }
    /*
        // Check if the user exists if appropriate.
        if ($op != 'create' && $op != 'register' ) {
            $account = user_load($args[0]);
            if (!$account) {
                return services_error(t('There is no user with ID @uid.', array('@uid' => $args[0])), 406);
            }
        }
    */
    global $user;
    $account = user_load($user->uid);
    switch ($op) {
        case 'view':
            return user_view_access($account);
        case 'update':
            return ($user->uid == $account->uid || user_access('administer users'));
        case 'create':
        case 'register':
            if (!$user->uid && variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL) != USER_REGISTER_ADMINISTRATORS_ONLY) {
                return TRUE;
            }
            else {
                return user_access('administer users');
            }
        case 'password_reset':
            return TRUE;
        case 'delete':
        case 'cancel':
        case 'resend_welcome_email':
            return user_access('administer users');
    }
}