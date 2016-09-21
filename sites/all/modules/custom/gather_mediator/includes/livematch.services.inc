<?php
function gather_mediator_livematch_services_resource() {
    if (!module_exists('comment')) {
        return array();
    }
    $api = array(
        'livematch_report' => array(
            'operations' => array(
                'retrieve' => array(
                    'help' => 'Live Match',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/livematch.services',
                    ),
                    'callback' => 'gather_mediator_livematch_report_resource_retrieve',
                    'access arguments' => array('access content'),
                    //'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'nid',
                            'optional' => TRUE,
                            'type' => 'int',
                            'description' => 'The zero-based index of the page to get, defaults to 0.',
                            'default' => '0',
                            'source' => array('param' => 'nid'),
                        ),
                    ),

                ),
                'create' => array(
                    'help' => 'Create a Report for live match',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/livematch.services',
                    ),
                    'callback' => 'gather_mediator_livematch_report_resource_create',
                    'access callback' => 'livemach_report_services_resource_access',
                    'access arguments' => array('create'),
                    'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'comment',
                            'type' => 'array',
                            'description' => 'create the matches.',
                            'source' => 'data',
                            'optional' => FALSE,
                        ),
                    ),

                ),
                'update' => array(
                    'help' => 'Update a Report',
                    'file' => array('type' => 'inc', 'module' => 'gather_mediator', 'name' => 'includes/livematch.services'),
                    'callback' => 'gather_mediator_livematch_report_resource_update',
                    'access callback' => 'livemach_report_services_resource_access',
                    'access arguments' => array('edit'),
                    'access arguments append' => TRUE,
                    'args' => array(
                        array(
                            'name' => 'cid',
                            'optional' => FALSE,
                            'source' => array('path' => 0),
                            'type' => 'int',
                            'description' => 'The unique identifier for this comment.',
                        ),
                        array(
                            'name' => 'data',
                            'type' => 'array',
                            'description' => 'The comment object with updated information',
                            'source' => 'data',
                            'optional' => FALSE,
                        ),
                    ),
                ),
                'delete' => array(
                    'help' => 'Delete a comment',
                    'file' => array('type' => 'inc', 'module' => 'gather_mediator', 'name' => 'includes/livematch.services'),
                    'callback' => 'gather_mediator_livematch_report_resource_delete',
                    'access callback' => 'livemach_report_services_resource_access',
                    'access arguments' => array('edit'),
                    'access arguments append' => TRUE,
                    'args' => array(
                        array(
                            'name' => 'cid',
                            'type' => 'int',
                            'description' => 'The id of the comment to delete',
                            'source' => array('path' => '0'),
                            'optional' => FALSE,
                        ),
                    ),
                ),

            ),
        ),

    );


    return $api;
}



function gather_mediator_livematch_report_resource_retrieve($nid){

    //print_r($nid);die();
   //return $comment;

}

function gather_mediator_livematch_report_resource_create($comment)
{

    if(!empty($comment['points']) || !empty($comment['goals'])):

    $node_obj = node_load($comment['match_id']);
    $language = $node_obj->language;

    foreach ($node_obj->field_club_detail[$language] as $field_collection) {
        // Load field collection item.
        $club_detail = field_collection_field_get_entity($field_collection);


        if($club_detail->field_home_club_id[$language][0]['value'] == $comment['team_id']):
            $home_club_points = $comment['points'];
            $home_club_goals = $comment['goals'];
            $club_obj = node_load($club_detail->field_home_club_id['und'][0]['value']);
            $team_img = file_create_url($club_obj->field_club_image['und'][0]['uri']);
            if(!empty($comment['points'])) {
             $club_detail->field_home_club_points[$language][0]['value'] = $comment['points'];
                $club_detail->save(TRUE);
            }
            elseif(!empty($comment['goals'])) {
                $club_detail->field_home_club_goals[$language][0]['value'] = $comment['goals'];
                $club_detail->save(TRUE);
            }

        endif;
        if($club_detail->field_opponent_club_id[$language][0]['value'] == $comment['team_id']):

            $opp_club_points = $comment['points'];
            $opp_club_goals = $comment['goals'];
            $club_obj = node_load($club_detail->field_opponent_club_id['und'][0]['value']);
            $team_img = file_create_url($club_obj->field_club_image['und'][0]['uri']);

            if(!empty($comment['points'])) {

            $club_detail->field_opponent_club_points[$language][0]['value'] = $comment['points'];
                $club_detail->save(TRUE);
        }elseif(!empty($comment['goals'])) {
            $club_detail->field_opponent_club_goals[$language][0]['value'] = $comment['goals'];
            $club_detail->save(TRUE);
        }
        endif;

    }
   endif;
    // Adds backwards compatability with regression fixed in #1083242
    $comment = _services_arg_value($comment, 'comment');

    if (empty($comment['nid'])) {
        return services_error(t('A nid must be provided'));
    }

    $form_state['values'] = $comment;
    $form_state['values']['op'] = variable_get('services_comment_save_button_resource_create', t('Save'));

    $comment_empty = array(
        'nid' => $comment['nid'],
        'cid' => NULL,
    );

    // If a pid is provide use it
    if (!empty($comment['pid'])) {
        $comment_empty['pid'] = $comment['pid'];
    }

    $comment_empty = (object) $comment_empty;
    $form_state['programmed_bypass_access_check'] = FALSE;
    $ret = drupal_form_submit('comment_form', $form_state, $comment_empty);

    // Error if needed.
    if ($errors = form_get_errors()) {
        return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
    }
    $comment = $form_state['comment'];
    //print_r($comment);die;
    return array(
        'report_id' => $comment->cid,
        'match_id' => $comment->nid,
        'user_name' => $comment->name,
        'report_title' => $comment->subject,
        'report_detail' => $comment->comment_body['und'][0]['value'],
        'comment' => $comment->field_comment['und'][0]['value'],
        'team_id' => $comment->field_team_id['und'][0]['value'],
        'team_image' => $team_img,
        'home_team_points' => $home_club_points,
        'home_team_goals' => $home_club_goals,
        'opp_team_points' => $opp_club_points,
        'opp_team_goals' => $opp_club_goals,
        'report_created' => $comment->created,
        'report_last_updated' => $comment->changed,
        'last_commented_time' => $comment->field_last_commented_time['und'][0]['value'],
        'substitute' => $comment->field_substitute['und'][0]['value']
    );

}

/**
 * Updates a comment and returns the cid.
 *
 * @param $cid
 *   Unique identifier for this comment.
 * @param $comment
 *   An object as would be returned from comment_load().
 * @return
 *   Unique identifier for the comment (cid) or FALSE if there was a problem.
 */
function gather_mediator_livematch_report_resource_update($cid, $comment) {

    // Adds backwards compatability with regression fixed in #1083242
    $comment = _services_arg_value($comment, 'data');
    $comment['cid'] = $cid;
    $old_comment = comment_load($cid);
    if (empty($old_comment)) {
        return services_error(t('Comment @cid not found', array('@cid' => $cid)), 404);
    }
    // Setup form_state.
    $form_state = array();
    $form_state['values'] = $comment;
    $form_state['values']['op'] = variable_get('services_comment_save_button_resource_update', t('Save'));
    $form_state['comment'] = $old_comment;
    $form_state['programmed_bypass_access_check'] = FALSE;

    drupal_form_submit('comment_form', $form_state, $old_comment);

    if ($errors = form_get_errors()) {
        return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
    }

    return array(
        'report_id' => $cid,
        'match_id' => $comment['match_id'],
        'report_title' => $comment['report_title'],
        'report_detail' => $comment['report_detail']

    );
}


/**
 * Delete a comment.
 *
 * @param $cid
 *   Unique identifier of the comment to delete.
 * @return
 *   True.
 */
function gather_mediator_livematch_report_resource_delete($cid) {
    // Load in the required includes for comment_delete.
    module_load_include('inc', 'comment', 'comment.admin');

    // The following is from comment_confirm_delete_submit in comment.admin.inc
    $comment = comment_load($cid);
    if (empty($comment)) {
        return services_error(t('There is no comment found with id @cid', array('@cid' => $cid)), 404);
    }

    // Delete comment and its replies.
    comment_delete($cid);

    // Clear the cache so an anonymous user sees that his comment was deleted.
    cache_clear_all();
    return TRUE;
}

function livemach_report_services_resource_access($op = 'view', $args = array()) {
    // Adds backwards compatability with regression fixed in #1083242


    if (isset($args[0])) {
        $args[0] = _services_access_value($args[0], array('comment', 'data'));
    }

    if ($op == 'create') {
        $comment = (object)$args[0];
    }
    else {
        $comment = comment_load($args[0]);
    }
    if(isset($comment->nid)) {
        $node = node_load($comment->nid);
        if($op == 'create' && !$node->nid) {
            return services_error(t('Node nid: @nid does not exist.', array('@nid' => $comment->nid)), 406);
        }
    }
    if (user_access('administer comments')) {
        return TRUE;
    }
    switch ($op) {
        case 'view':
            // Check if the user has access to comments
            return user_access('access comments');
        case 'delete':
            return user_access('administer comments');
        case 'edit':
            return comment_access('edit', $comment);
        case 'create':
            // Check if the user may post comments, node has comments enabled
            // and that the user has access to the node.
           return user_access('post comments');
    }
}