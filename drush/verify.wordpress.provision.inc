<?php

/**
 * @file
 *
 * CiviCRM Ansible support module for the Aegir backend.
 *
 * This file contains the wordpress-verify operations.
 */

/**
 * Implements hook_post_provision_wordpress_verify().
 *
 * Run CiviCRM Ansible playbooks.
 */
function drush_provision_civicrm_ansible_post_provision_wordpress_verify($url = NULL) {
  if (d()->type == 'wpsite') {
    drush_log(dt("CiviCRM Ansible: running aegir-ansible !site", ['!site' => d()->uri]), 'ok');

    ob_start();
    passthru('sudo /usr/local/bin/aegir-ansible ' . escapeshellarg(d()->uri));
    $output = ob_get_clean();

    drush_log(dt("CiviCRM Ansible: !output", ['!output' => '<pre>' . $output . '</pre>']), 'ok');
  }
}
